#include "dlpc350/controller.hpp"
#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>

using namespace std::chrono_literals;

namespace DLPC350 {

bool Controller::open() {
  if (!USB::open()) {
    std::cerr << "[Controller] Unable to open devices" << std::endl;
    return false;
  }

  std::cout << "[Controller] Opening device connections: " << deviceNum()
            << std::endl;

  for (int i = 0; i < deviceNum(); ++i) {
    projectors.emplace_back(i);
  }

  return true;
}

void Controller::close() {
  std::cout << "[Controller] Closing device connections" << std::endl;
  projectors.clear();
  USB::close();
}

// TODO: use expected or optional to handle error cases
Projector &Controller::getProjector(unsigned int index) {
  assert(index < deviceNum());
  return projectors[index];
}

bool Controller::setPowerMode(PowerMode powerMode) {
  if (projectors.empty()) {
    std::cout << "[Controller] No projectors connected" << std::endl;
    return true;
  }

  for (auto &projector : projectors) {
    if (projector.controlled) {
      USB::select(projector.index);
      if (!DLPC350::setPowerMode(powerMode)) {
        std::cerr << "[Controller] Failed to set power mode" << std::endl;
        return false;
      }
      projector.powerMode = powerMode;
    }
  }

  std::this_thread::sleep_for(2000ms);

  return true;
}

bool Controller::setDisplayMode(DisplayMode displayMode) {
  if (projectors.empty()) {
    std::cout << "[Controller] No projectors connected" << std::endl;
    return true;
  }

  for (auto &projector : projectors) {
    if (projector.controlled) {
      USB::select(projector.index);
      if (!Controller::setDisplayModeSingle(displayMode)) {
        std::cerr << "[Controller] Failed to set display mode" << std::endl;
        return false;
      }
      projector.displayMode = displayMode;
    }
  }
  return true;
}

bool Controller::setDisplayModeSingle(DisplayMode displayMode) {
  auto currentDisplayMode = DLPC350::getDisplayMode();

  // If device is already in pattern mode, stop sequence
  if (*currentDisplayMode == DisplayMode::PATTERN) {
    auto patternSequenceStatus = DLPC350::getPatternSequenceStatus();
    if (*patternSequenceStatus != PatternSequenceStatus::STOP) {
      setPatternSequenceStatus(PatternSequenceStatus::STOP);
    }
  }

  if (*currentDisplayMode == displayMode) {
    return true;
  }

  DLPC350::setDisplayMode(displayMode);

  for (int i = 0; i < maxRetries; ++i) {
    std::this_thread::sleep_for(100ms);

    auto newDisplayMode = DLPC350::getDisplayMode();
    if (*newDisplayMode == displayMode) {
      return true;
    }
  }

  std::cerr << "[Controller] Exceeded max retries on display mode change"
            << std::endl;
  return false;
}

bool Controller::startPatternSequence(PatternSequence &patternSequence) {
  if (projectors.empty()) {
    std::cout << "[Controller] No projectors connected" << std::endl;
    return true;
  }

  for (auto &projector : projectors) {
    if (projector.controlled) {
      USB::select(projector.index);
      if (!Controller::startPatternSequenceSingle(patternSequence)) {
        std::cerr << "[Controller] Failed to start pattern sequence"
                  << std::endl;
        return false;
      }
      projector.patternSequenceStatus = PatternSequenceStatus::START;
    }
  }
  std::cout << "[Controller] Pattern sequence started" << std::endl;
  return true;
}

bool Controller::startPatternSequenceSingle(PatternSequence &patternSequence) {
  Controller::setDisplayModeSingle(DisplayMode::PATTERN);

  DLPC350::setPatternDataSource(PatternDataSource::EXTERNAL);
  DLPC350::setPatternPeriod(patternSequence.getExposure(),
                            patternSequence.getPeriod());
  DLPC350::setPatternTriggerMode(PatternTriggerMode::MODE0);

  Controller::sendPatternSequenceSingle(patternSequence);

  Controller::validatePatternSequenceSingle();

  return DLPC350::setPatternSequenceStatus(PatternSequenceStatus::START);
}

bool Controller::stopPatternSequence() {
  if (projectors.empty()) {
    std::cout << "[Controller] No projectors connected" << std::endl;
    return true;
  }

  for (auto &projector : projectors) {
    if (projector.controlled) {
      USB::select(projector.index);
      if (!Controller::stopPatternSequenceSingle()) {
        std::cerr << "[Controller] Failed to stop pattern sequence"
                  << std::endl;
        return false;
      }
      projector.patternSequenceStatus = PatternSequenceStatus::STOP;
    }
  }
  std::cout << "[Controller] Pattern Stopped" << std::endl;
  return true;
}

inline bool Controller::stopPatternSequenceSingle() {
  return setPatternSequenceStatus(PatternSequenceStatus::STOP);
}

bool Controller::sendPatternSequenceSingle(PatternSequence &patternSequence) {
  DLPC350::configurePatternSequence(patternSequence);
  return DLPC350::sendPatternDisplayLUT(patternSequence);
}

bool Controller::validatePatternSequenceSingle() {
  Controller::setPatternSequenceStatusSingle(PatternSequenceStatus::STOP);

  for (int i = 0; i < maxRetries; ++i) {
    auto validation = DLPC350::validatePatternSequence();
    if (validation->isValid()) {
      std::cout << "[Controller] Pattern Validated" << std::endl;
      return true;
    }
    std::this_thread::sleep_for(1000ms);
  }

  std::cerr << "[Controller] Exceed max retries on validate" << std::endl;
  return false;
}

bool Controller::setPatternSequenceStatusSingle(
    PatternSequenceStatus psStatus) {
  // TODO: may need to repeat inside for loop
  DLPC350::setPatternSequenceStatus(psStatus);

  for (int i = 0; i < maxRetries; ++i) {
    std::this_thread::sleep_for(100ms);

    auto currentStatus = DLPC350::getPatternSequenceStatus();
    if (*currentStatus == psStatus) {
      return true;
    }
  }

  std::cerr
      << "[Controller] Exceeded max retries on Pattern Sequence start/stop"
      << std::endl;
  return false;
}

bool Controller::setLEDCurrent(const std::vector<LEDCurrent> &currents) {
  if (currents.size() != projectors.size()) {
    std::cerr << "[Controller] Number of controlled projectors doesn't match "
                 "input argument."
              << std::endl;
    return false;
  }

  for (unsigned int i = 0; i < currents.size(); ++i) {
    auto &projector = projectors[i];
    if (projector.controlled) {
      Controller::setLEDCurrent(i, currents[i]);
    }
  }

  std::cout << "[Controller] LED currents configured" << std::endl;
  return true;
}

bool Controller::setLEDCurrent(unsigned int index, LEDCurrent ledCurrent) {
  assert(ledCurrent.red >= 0);
  assert(ledCurrent.red <= 255);
  assert(ledCurrent.green >= 0);
  assert(ledCurrent.green <= 255);
  assert(ledCurrent.blue >= 0);
  assert(ledCurrent.blue <= 255);

  if (index >= projectors.size()) {
    std::cerr << "[Controller] Index exceeds # of controlled projectors"
              << std::endl;
    return false;
  }

  if (!projectors[index].controlled) {
    return true;
  }

  USB::select(projectors[index].index);

  if (!DLPC350::setLEDCurrent(ledCurrent.red, ledCurrent.green,
                              ledCurrent.blue)) {
    return false;
  }

  projectors[index].ledCurrent = ledCurrent;

  std::this_thread::sleep_for(200ms);
  return true;
}

}; // namespace DLPC350