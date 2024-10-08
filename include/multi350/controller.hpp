#ifndef MULTI350_CONTROLLER_HPP
#define MULTI350_CONTROLLER_HPP

#include "dlpc350.hpp"
#include "message.hpp"
#include "pattern.hpp"
#include "status.hpp"
#include "usb.hpp"
#include <cstdint>
#include <vector>

namespace multi350 {

/// @brief Number of max possible retries on configuration attempt
constexpr unsigned int maxRetries = 10;

/// @brief Contains Projector status and index information
struct Projector {
  unsigned int index;
  bool controlled{true};
  PowerMode powerMode;
  LEDCurrent ledCurrent;
  DisplayMode displayMode;
  PatternStatus patternStatus;

  HardwareStatus hardwareStatus;
  SystemStatus systemStatus;
  MainStatus mainStatus;

  Projector()
      : index{0}, powerMode{PowerMode::NORMAL}, ledCurrent{0},
        displayMode{DisplayMode::VIDEO}, patternStatus(PatternStatus::STOP) {}
  Projector(unsigned int _index, PowerMode _powerMode = PowerMode::NORMAL,
            LEDCurrent _ledCurrent = LEDCurrent{0},
            DisplayMode _displayMode = DisplayMode::VIDEO,
            PatternStatus _psStatus = PatternStatus::STOP)
      : index{_index}, powerMode{_powerMode}, ledCurrent{_ledCurrent},
        displayMode{_displayMode}, patternStatus{_psStatus} {}
};

struct Controller {
  /// @brief Initialize the Controller
  /// @return True on success
  inline bool init() { return USB::init(); }

  /// @brief Free the Controller
  /// @return True on success
  inline bool exit() { return USB::exit(); }

  /// @brief Open USB connections to connected DLPC350 devices
  /// @return True on success
  bool open();

  /// @brief Close all USB connections to DLPC350 devices
  void close();

  /// @brief Sync the controller with the projectors
  void sync();

  /// @brief Check if there are connected DLPC350 devices
  /// @return True if there is at least one device connected
  inline bool isConnected() { return USB::isConnected(); }

  /// @brief Get number of DLPC350 devices connected
  /// @return Number of connected devices
  inline unsigned int deviceNum() { return USB::deviceNum(); }

  /// @brief Get reference to the stored projector object
  /// @param index Index to projector
  /// @return Reference to the Projector
  Projector &getProjector(unsigned int index);

  // TODO: change namespace for dlpc350 on cmake
  // TODO: adjust control flag check

  /// @brief Control all projectors
  void controlAll();

  /// @brief Control a single projector
  /// @param index index of projector to control
  void controlSingle(unsigned int index);

  // TODO: change to a swap to match imgui function?
  bool updateIndices(const std::vector<unsigned int> &indices);

  /// @brief Perform software reset on all controlled projectors
  /// @return True on success
  bool softwareReset();

  /// @brief Update hardware/main/system status of the projectors
  void updateStatus();

  /// @brief Set power mode on projectors. Waits 2000ms to finish switching.
  /// @param powerMode STANDBY(true) / NORMAL(false)
  /// @return True on success
  bool setPowerMode(PowerMode powerMode);

  /// @brief Set power mode for a single projector. Waits 2000ms to finish
  /// @param index index of projector
  /// @param powerMode STANDBY(true) / NORMAL(false)
  /// @return True on success
  bool setPowerMode(unsigned int index, PowerMode powerMode);

  /// @brief Start test pattern on controlled projectors
  /// @param testType type of test pattern
  /// @return True on success
  bool startTestPattern(TestPattern testType);

  /// @brief Stop test pattern and set parallel 24bit as input source
  /// @return True on success
  bool stopTestPattern();

  /// @brief Set display mode on all controlled projectors
  /// @param displayMode PATTERN(true) / VIDEO(false)
  /// @return True on success
  bool setDisplayMode(DisplayMode displayMode);

  /// @brief Start video mode on all controlled projectors
  /// @return True on success
  bool startVideoMode();

  /// @brief Start pattern sequence on all controlled projectors. Should create
  /// necessary pattern sequence object and add patterns prior to calling this
  /// function.
  /// @param PatternSequence Reference to pattern sequence object
  /// @return True on success
  bool startPatternSequence(PatternSequence &patternSequence);

  /// @brief Start variable exposure pattern sequence on all controlled
  /// projectors. shoudl create necessary variable exposure patterns prior to
  /// calling this function.
  /// @param varExpPatSequence Reference to variable exposure pattern sequence
  /// object
  /// @return True on success
  bool startVarExpPatSequence(VarExpPatSequence &varExpPatSequence);

  /// @brief Stop pattern sequence on all controlled projectors.
  /// @return True on success
  bool stopPatternSequence();

  /// @brief Set LED currents on all controlled projectors
  /// @param currents std::vector containing LEDCurrent objects matching the
  /// current controlled projectors
  /// @return True on success
  bool setLEDCurrent(const std::vector<LEDCurrent> &currents);

  /// @brief Set LED currents on the specificied projector
  /// @param index index of the projector in the controller (may not match the
  /// index on usb class)
  /// @param ledCurrent LEDCurrent object containing current values
  /// @return True on success
  bool setLEDCurrent(unsigned int index, LEDCurrent ledCurrent);

  /// @brief Prints all list of connected devices
  inline void printDevices() { USB::printDevices(); }

  /// @brief Prints all the status of connected devices
  void printStatus();

  // void showVersion();

protected:
  /// @brief Set which DLPC350 device is currently getting controlled by the
  /// HIDAPI library. Only needed for manually configuring a single specific
  /// projector.
  /// @param index Index of the projector (Should be less than deviceNum())
  /// @return True on success
  inline bool select(unsigned int index) { return USB::select(index); }

  /// @brief Set display mode for a single projector.
  /// @param displayMode PATTERN(true) / VIDEO(false)
  /// @return True on success
  bool setDisplayModeSingle(DisplayMode displayMode);

  /// @brief Start pattern sequence on a single projector. Should create
  /// necessary pattern sequence object and add patterns prior to calling this
  /// function.
  /// @param patternSequence  Reference to pattern sequence object
  /// @return True on success
  bool startPatternSequenceSingle(PatternSequence &patternSequence);

  /// @brief Start variable exposure pattern sequence on a single projector.
  /// Should create necessary variable exposure pattern sequence objects prior
  /// to calling this function.
  /// @param varExpPatSequence  Reference to variable exposure pattern sequence
  /// object
  /// @return True on success
  bool startVarExpPatSequenceSingle(VarExpPatSequence &varExpPatSequence);

  /// @brief Validate the current pattern configured on the DLPC350. Expects the
  /// pattern data and the related configuration to be already set.
  /// @return True on success
  bool validatePatternSequenceSingle();

  /// @brief Start/Stop the pattern sequence. Expects the pattern sequence to be
  /// validated before calling this function.
  /// @param psStatus PatternStatus object indicating start/stop
  /// @return True on success
  bool setPatternStatusSingle(PatternStatus psStatus);

  /// @brief Contains information of connected projectors and the corresponding
  /// index for the USB interface.
  std::vector<Projector> projectors;
};

}; // namespace multi350
#endif