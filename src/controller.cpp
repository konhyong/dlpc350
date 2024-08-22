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

  for (int i = 0; i < deviceNum(); ++i) {
    projectors.emplace_back(i);
  }

  Controller::sync();

  std::cout << "[Controller] Opening device connections: " << deviceNum()
            << std::endl;

  return true;
}

void Controller::close() {
  std::cout << "[Controller] Closing device connections" << std::endl;
  projectors.clear();
  USB::close();
}

void Controller::sync() {
  if (projectors.empty()) {
    std::cout << "[Controller] No projectors connected" << std::endl;
    return;
  }

  for (auto &projector : projectors) {
    USB::select(projector.index);

    projector.powerMode = *DLPC350::getPowerMode();
    projector.ledCurrent = *DLPC350::getLEDCurrent();
    projector.displayMode = *DLPC350::getDisplayMode();
    projector.patternStatus = *DLPC350::getPatternStatus();

    projector.hardwareStatus = *DLPC350::getHardwareStatus();
    projector.systemStatus = *DLPC350::getSystemStatus();
    projector.mainStatus = *DLPC350::getMainStatus();
  }
}

void Controller::controlAll() {
  std::cout << "[Controller] Controlling all projectors" << std::endl;
  for (auto &projector : projectors) {
    projector.controlled = true;
  }
}

void Controller::controlSingle(unsigned int index) {
  assert(index < deviceNum());
  std::cout << "[Controller] Controlling projector " << index << std::endl;
  for (unsigned int i = 0; i < projectors.size(); ++i) {
    if (i == index) {
      projectors[i].controlled = true;
    } else {
      projectors[i].controlled = false;
    }
  }
}

bool Controller::updateIndices(const std::vector<unsigned int> &indices) {
  if (indices.size() != Controller::deviceNum()) {
    std::cerr << "[Controller] indices don't match connected devices"
              << std::endl;
    return false;
  }

  for (unsigned int i = 0; i < indices.size(); ++i) {
    projectors[i].index = indices[i];
  }

  return true;
}

// TODO: use expected or optional to handle error cases
Projector &Controller::getProjector(unsigned int index) {
  assert(index < deviceNum());
  return projectors[index];
}

bool Controller::softwareReset() {
  if (projectors.empty()) {
    std::cout << "[Controller] No projectors connected" << std::endl;
    return false;
  }

  for (auto &projector : projectors) {
    if (projector.controlled) {
      USB::select(projector.index);

      if (!DLPC350::softwareReset()) {
        std::cerr << "[Controller] Unable to send reset message" << std::endl;
        return false;
      }
    }
  }

  return true;
}

void Controller::updateStatus() {
  if (projectors.empty()) {
    std::cout << "[Controller] No projectors connected" << std::endl;
    return;
  }

  for (auto &projector : projectors) {
    if (projector.controlled) {
      USB::select(projector.index);

      projector.hardwareStatus = *DLPC350::getHardwareStatus();
      projector.systemStatus = *DLPC350::getSystemStatus();
      projector.mainStatus = *DLPC350::getMainStatus();
    }
  }
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

  std::cout << "[Controller] Set Power Mode: "
            << ((powerMode == PowerMode::NORMAL) ? "Normal" : "Standby")
            << std::endl;

  return true;
}

bool Controller::setPowerMode(unsigned int index, PowerMode powerMode) {
  if (projectors.empty()) {
    std::cout << "[Controller] No projectors connected" << std::endl;
    return true;
  }

  assert(index < deviceNum());
  auto &projector = projectors[index];

  USB::select(projector.index);
  if (!DLPC350::setPowerMode(powerMode)) {
    std::cerr << "[Controller] Failed to set power mode" << std::endl;
    return false;
  }
  projector.powerMode = powerMode;

  std::this_thread::sleep_for(2000ms);

  std::cout << "[Controller] Set Power Mode: "
            << ((powerMode == PowerMode::NORMAL) ? "Normal" : "Standby")
            << std::endl;

  return true;
}

bool Controller::startTestPattern(TestPattern testType) {
  if (projectors.empty()) {
    std::cout << "[Controller] No projectors connected" << std::endl;
    return true;
  }

  for (auto &projector : projectors) {
    if (projector.controlled) {
      USB::select(projector.index);
      if (!DLPC350::setTestPattern(testType)) {
        std::cerr << "[Controller] Failed to set test pattern" << std::endl;
        return false;
      }
      if (!DLPC350::setInputSource(InputType::TEST_PATTERN,
                                   InputBitDepth::INTERNAL)) {
        std::cerr << "[Controller] Failed to set input source to test pattern"
                  << std::endl;
        return false;
      }
    }
  }

  return true;
}

bool Controller::stopTestPattern() {
  if (projectors.empty()) {
    std::cout << "[Controller] No projectors connected" << std::endl;
    return true;
  }

  for (auto &projector : projectors) {
    if (projector.controlled) {
      USB::select(projector.index);
      if (!DLPC350::setInputSource(InputType::PARALLEL,
                                   InputBitDepth::BITS24)) {
        std::cerr << "[Controller] Failed to set input source to parallel 24bit"
                  << std::endl;
        return false;
      }
    }
  }

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
    auto patternStatus = DLPC350::getPatternStatus();
    if (*patternStatus != PatternStatus::STOP) {
      if (!Controller::setPatternStatusSingle(PatternStatus::STOP)) {
        return false;
      }
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

bool Controller::startVideoMode() {
  std::cout << "[Controller] Set display mode: Video" << std::endl;
  return Controller::setDisplayMode(DisplayMode::VIDEO);
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
      projector.patternStatus = PatternStatus::START;
    }
  }

  std::cout << "[Controller] Set display mode: Pattern" << std::endl;
  return true;
}

bool Controller::startPatternSequenceSingle(PatternSequence &patternSequence) {
  if (!Controller::setDisplayModeSingle(DisplayMode::PATTERN)) {
    return false;
  }

  if (!DLPC350::setPatternDataSource(PatternDataSource::EXTERNAL)) {
    std::cerr << "[Controller] Failed to set pattern data source" << std::endl;
    return false;
  }

  if (!DLPC350::configurePatternSequence(patternSequence)) {
    std::cerr << "[Controller] Failed to configure pattern sequence"
              << std::endl;
    return false;
  }

  if (!DLPC350::setPatternTriggerMode(PatternTriggerMode::MODE0)) {
    std::cerr << "[Controller] Failed to set pattern trigger mode" << std::endl;
    return false;
  }

  if (!DLPC350::setPatternPeriod(patternSequence.getExposure(),
                                 patternSequence.getPeriod())) {
    std::cerr << "[Controller] Failed to set pattern period" << std::endl;
    return false;
  }

  if (!DLPC350::sendPatternDisplayLUT(patternSequence)) {
    std::cerr << "[Controller] Failed to send pattern sequence to LUT"
              << std::endl;
    return false;
  }

  if (!Controller::validatePatternSequenceSingle()) {
    return false;
  }

  return Controller::setPatternStatusSingle(PatternStatus::START);
}

bool Controller::startVarExpPatSequence(VarExpPatSequence &varExpPatSequence) {
  if (projectors.empty()) {
    std::cout << "[Controller] No projectors connected" << std::endl;
    return true;
  }

  for (auto &projector : projectors) {
    if (projector.controlled) {
      USB::select(projector.index);
      if (!Controller::startVarExpPatSequenceSingle(varExpPatSequence)) {
        std::cerr
            << "[Controller] Failed to start variable exposure pattern sequence"
            << std::endl;
        return false;
      }
      projector.patternStatus = PatternStatus::START;
    }
  }

  std::cout << "[Controller] Set display mode: Pattern" << std::endl;
  return true;
}

bool Controller::startVarExpPatSequenceSingle(
    VarExpPatSequence &varExpPatSequence) {
  if (!Controller::setDisplayModeSingle(DisplayMode::PATTERN)) {
    return false;
  }

  if (!DLPC350::setPatternDataSource(PatternDataSource::EXTERNAL)) {
    std::cerr << "[Controller] Failed to set pattern data source" << std::endl;
    return false;
  }

  if (!DLPC350::setPatternTriggerMode(PatternTriggerMode::MODE4)) {
    std::cerr << "[Controller] Failed to set pattern trigger mode" << std::endl;
    return false;
  }

  if (!DLPC350::configureVarExpPatSequence(varExpPatSequence)) {
    std::cerr
        << "[Controller] Failed to configure variable exposure pattern sequence"
        << std::endl;
    return false;
  }

  if (!DLPC350::sendVarExpPatDisplayLUT(varExpPatSequence)) {
    std::cerr << "[Controller] Failed to send variable exposure pattern "
                 "sequence to LUT"
              << std::endl;
    return false;
  }

  if (!Controller::validatePatternSequenceSingle()) {
    return false;
  }

  return Controller::setPatternStatusSingle(PatternStatus::START);
}

bool Controller::stopPatternSequence() {
  if (projectors.empty()) {
    std::cout << "[Controller] No projectors connected" << std::endl;
    return true;
  }

  for (auto &projector : projectors) {
    if (projector.controlled) {
      USB::select(projector.index);
      if (!Controller::setPatternStatusSingle(PatternStatus::STOP)) {
        std::cerr << "[Controller] Failed to stop pattern sequence"
                  << std::endl;
        return false;
      }
      projector.patternStatus = PatternStatus::STOP;
    }
  }
  std::cout << "[Controller] Pattern Stopped" << std::endl;
  return true;
}

bool Controller::validatePatternSequenceSingle() {
  Controller::setPatternStatusSingle(PatternStatus::STOP);

  DLPC350::startPatternValidation();

  auto checkBusy = DLPC350::checkPatternValidation();
  if (checkBusy->isReady()) {
    std::cerr << "[Controller] Validation command not executed properly"
              << std::endl;
    return false;
  }

  for (int i = 0; i < maxRetries; ++i) {
    auto validation = DLPC350::checkPatternValidation();
    if (validation->isReady()) {
      if (validation->isValid()) {
        return true;
      } else {
        std::cerr << "[Controller] Pattern failed to validate" << std::endl;
        return false;
      }
    }
    std::this_thread::sleep_for(100ms);
  }

  std::cerr << "[Controller] Exceed max retries on validate" << std::endl;
  return false;
}

bool Controller::setPatternStatusSingle(PatternStatus psStatus) {

  DLPC350::setPatternStatus(psStatus);

  for (int i = 0; i < maxRetries; ++i) {
    std::this_thread::sleep_for(100ms);

    auto currentStatus = DLPC350::getPatternStatus();
    if (*currentStatus == psStatus) {
      return true;
    }
    DLPC350::setPatternStatus(psStatus);
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

void Controller::printStatus() {
  for (auto &projector : projectors) {
    std::cout << "[Projector " << projector.index << "]" << std::endl;
    std::cout << " controlled: " << projector.controlled << std::endl;
    std::cout << " powerMode: " << static_cast<bool>(projector.powerMode)
              << std::endl;
    std::cout << " ledCurrent: "
              << static_cast<unsigned int>(projector.ledCurrent.red) << ", "
              << static_cast<unsigned int>(projector.ledCurrent.green) << ", "
              << static_cast<unsigned int>(projector.ledCurrent.blue)
              << std::endl;
    std::cout << " displayMode: " << static_cast<bool>(projector.displayMode)
              << std::endl;
    std::cout << " patternStatus: "
              << static_cast<unsigned int>(projector.patternStatus)
              << std::endl;

    std::cout << "\n HardwareStatus) " << std::endl;
    std::cout << "  initError: " << projector.hardwareStatus.initError
              << std::endl;
    std::cout << "  DRCError: " << projector.hardwareStatus.DRCError
              << std::endl;
    std::cout << "  forcedSwap: " << projector.hardwareStatus.forcedSwap
              << std::endl;
    std::cout << "  sequenceAbort: " << projector.hardwareStatus.sequenceAbort
              << std::endl;
    std::cout << "  sequenceError: " << projector.hardwareStatus.sequenceError
              << std::endl;

    std::cout << "\n SystemStatus) " << std::endl;
    std::cout << "  memoryTest: " << projector.systemStatus.memoryTest
              << std::endl;

    std::cout << "\n MainStatus) " << std::endl;
    std::cout << "  DMDParked: " << projector.mainStatus.DMDParked << std::endl;
    std::cout << "  sequenceRunning: " << projector.mainStatus.sequenceRunning
              << std::endl;
    std::cout << "  bufferFrozen: " << projector.mainStatus.bufferFrozen
              << std::endl;
    std::cout << "  gammaCorrection: " << projector.mainStatus.gammaCorrection
              << std::endl;
  }
}

}; // namespace DLPC350