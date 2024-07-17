#ifndef DLPC350_DLPC350_HPP
#define DLPC350_DLPC350_HPP

#include "pattern.hpp"
#include <cstdint>
#include <memory>
#include <string>

namespace DLPC350 {

union HardwareStatus {
  uint8_t value;
  struct {
    bool initError : 1;     // 0 : error
    uint8_t : 1;            // reserved
    bool DRCError : 1;      // 1 : error
    bool forcedSwap : 1;    // 1 : error
    uint8_t : 2;            // reserved
    bool sequenceAbort : 1; // 1 : error
    bool sequenceError : 1; // 1 :: error
  };
  HardwareStatus() : value{0} {}
};

union SystemStatus {
  uint8_t value;
  struct {
    bool memoryTest : 1; // 0 : error
    uint8_t : 7;         // reserved
  };
  SystemStatus() : value{0} {}
};

union MainStatus {
  uint8_t value;
  struct {
    bool DMDParked : 1;       // 1 : parked
    bool sequenceRunning : 1; // 1 : running
    bool bufferFrozen : 1;    // 1 : frozen
    bool gammaCorrection : 1; // 1 : enabled
    uint8_t : 4;
  };
  MainStatus() : value{0} {}
};

struct Version {
  union {
    uint32_t value;
    struct {
      uint16_t patch : 16;
      uint8_t minor : 8;
      uint8_t major : 8;
    };
  } app, api, softwareConfig, sequenceConfig;
  Version() {}
  Version(uint32_t _app, uint32_t _api, uint32_t _sw, uint32_t _seq)
      : app{_app}, api{_api}, softwareConfig{_sw}, sequenceConfig{_seq} {}
};

enum class PowerMode : bool {
  STANDBY = true, // Standby Mode
  NORMAL = false  // Normal Operation
};

struct CurtainColor {
  uint16_t red : 10;
  uint16_t green : 10;
  uint16_t blue : 10;

  CurtainColor() : red{0}, green{0}, blue{0} {}
  CurtainColor(uint16_t _red, uint16_t _green, uint16_t _blue)
      : red{_red}, green{_green}, blue{_blue} {}
};

enum class InputType : uint8_t {
  PARALLEL = 0,
  TEST_PATTERN = 1,
  FLASH = 2,
  FPDLINK = 3
};

enum class InputBitDepth : uint8_t {
  INTERNAL = 0,
  BITS30 = 0,
  BITS24 = 1,
  BITS20 = 2,
  BITS16 = 3,
  BITS10 = 4,
  BITS8 = 5
};

union InputSource {
  uint8_t value;
  struct {
    InputType type : 3;
    InputBitDepth bitDepth : 3;
    uint8_t : 2;
  };
  InputSource() : value{0} {}
  InputSource(uint8_t _value) : value{_value} {}
  InputSource(InputType _type, InputBitDepth _bitDepth)
      : type{_type}, bitDepth{_bitDepth} {}
};

enum class TestPattern : uint8_t {
  SOLID_FIELD = 0,
  HORIZONTAL_RAMP = 1,
  VERTICAL_RAMP = 2,
  HORIZONTAL_LINES = 3,
  DIAGONAL_LINES = 4,
  VERTICAL_LINES = 5,
  GRID = 6,
  CHECKERBOARD = 7,
  RGB_RAMP = 8,
  COLOR_BARS = 9,
  STEP_BARS = 10
};

enum class LEDEnableMode : bool {
  AUTO = true,   // LED enables are controlled by sequencer
  MANUAL = false // controlled by LED enable bits
};

union LEDEnable {
  uint8_t value;
  struct {
    bool red : 1;
    bool green : 1;
    bool blue : 1;
    LEDEnableMode mode : 1;
    uint8_t : 4;
  };
  LEDEnable() : value{0} {}
  LEDEnable(uint8_t _value) : value{_value} {}
  LEDEnable(LEDEnableMode _mode, bool _red, bool _green, bool _blue)
      : mode{_mode}, red{_red}, green{_green}, blue{_blue} {}
};

union LEDCurrent {
  uint32_t value;
  struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
  };
  LEDCurrent() : value{0} {}
  LEDCurrent(uint32_t _value) : value{_value} {}
  LEDCurrent(uint8_t _red, uint8_t _green, uint8_t _blue)
      : red{_red}, green{_green}, blue{_blue} {}
};

enum class DisplayMode : bool {
  PATTERN = true, // Pattern Display
  VIDEO = false   // Video Display
};

union GammaCorrection {
  uint8_t value;
  struct {
    bool degammaTable : 1; // 0 = TI Video (Enhanced)
    uint8_t : 6;
    bool enable : 1;
  };
  GammaCorrection() : value{0} {}
  GammaCorrection(uint8_t _value) : value{_value} {}
  GammaCorrection(bool _degammaTable, bool _enable)
      : degammaTable(_degammaTable), enable(_enable) {}
};

union PatternSequenceValidation {
  uint8_t value;
  struct {
    bool invalidPeriod : 1;
    bool invalidPattern : 1;
    bool invalidTriggerOut1 : 1;
    bool invalidPostVector : 1;
    bool invalidPeriodDifference : 1;
    uint8_t : 2;
    bool busy : 1;
  };
  PatternSequenceValidation() : value{0} {}
  PatternSequenceValidation(uint8_t _value) : value(_value) {}
  inline bool isValid() { return (value & ((1 << 5) - 1)) == 0; }
  inline bool isReady() { return !busy; }
};

enum class PatternTriggerMode : uint8_t {
  MODE0 = 0, // Pattern Trigger Mode 0: VSYNC serves to trigger the pattern
             // display sequence.
  MODE1 = 1, // Pattern Trigger Mode 1: Internally or Externally (through
             // TRIG_IN1 and TRIG_IN2) generated trigger.
  MODE2 = 2, // Pattern Trigger Mode 2: TRIG_IN_1 alternates between two
             // patterns,while TRIG_IN_2 advances to the next pair of patterns.
  MODE3 = 3, // Pattern Trigger Mode 3: Internally or externally generated
             // trigger for Variable Exposure display sequence.
  MODE4 = 4  // Pattern Trigger Mode 4: VSYNC triggered for Variable Exposure
             // display sequence.
};

enum class PatternDataSource : uint8_t {
  EXTERNAL = 0, // Video port(24-bit RGB / FPD-Link)
  RESERVED1 = 1,
  RESERVED2 = 2,
  INTERNAL = 3 // Internal flash
};

enum class PatternSequenceStatus : uint8_t {
  STOP = 0,  // Next start will restart sequence from beginning
  PAUSE = 1, // Next start will restart sequence from current pattern
  START = 2  // Start pattern display sequence
};

struct PatternPeriod {
  uint32_t exposure; // Pattern exposure time (us)
  uint32_t period;   // period period (us)

  PatternPeriod() : exposure{0x4010}, period{0x411A} {}
  PatternPeriod(uint32_t _exposure, uint32_t _period)
      : exposure{_exposure}, period{_period} {}
};

enum class MailboxMode : uint8_t {
  DISABLE = 0,             // Disable(Close) mailbox
  IMAGE_INDEX = 1,         // Open mailbox for image index configuration
  PATTERN = 2,             // Open mailbox for pattern definition
  VAR_EXPOSURE_PATTERN = 3 // Open mailbox for var exposure pattern definition
};

/// Status Commands
std::unique_ptr<HardwareStatus> getHardwareStatus();
std::unique_ptr<SystemStatus> getSystemStatus();
std::unique_ptr<MainStatus> getMainStatus();
std::unique_ptr<Version> getVersion();
std::unique_ptr<std::string> getFirmwareTag();

/// Chipset Control Commands
bool softwareReset();

std::unique_ptr<PowerMode> getPowerMode();
bool setPowerMode(PowerMode mode);

std::unique_ptr<CurtainColor> getColorCurtain();
bool setColorCurtain(uint16_t red, uint16_t green, uint16_t blue);

std::unique_ptr<InputSource> getInputSource();
bool setInputSource(InputType type,
                    InputBitDepth bitDepth = InputBitDepth::INTERNAL);

std::unique_ptr<TestPattern> getTestPattern();
bool setTestPattern(TestPattern pattern);

std::unique_ptr<LEDEnable> getLEDEnable();
bool setLEDEnable(LEDEnableMode mode, bool redEnabled = true,
                  bool greenEnabled = true, bool blueEnabled = true);

std::unique_ptr<LEDCurrent> getLEDCurrent();
bool setLEDCurrent(uint8_t red, uint8_t green, uint8_t blue);

/// Display Sequences
std::unique_ptr<DisplayMode> getDisplayMode();
bool setDisplayMode(DisplayMode mode);

std::unique_ptr<GammaCorrection> getGammaCorrection();
bool setGammaCorrection(bool enable, bool degammaTable = false);

std::unique_ptr<PatternSequenceValidation> validatePatternSequence();

std::unique_ptr<PatternTriggerMode> getPatternTriggerMode();
bool setPatternTriggerMode(PatternTriggerMode mode);

std::unique_ptr<PatternDataSource> getPatternDataSource();
bool setPatternDataSource(PatternDataSource input);

std::unique_ptr<PatternSequenceStatus> getPatternSequenceStatus();
bool setPatternSequenceStatus(PatternSequenceStatus mode);

std::unique_ptr<PatternPeriod> getPatternPeriod();
bool setPatternPeriod(uint32_t exposure, uint32_t frame);

bool setMailboxMode(MailboxMode mode);
bool setMailboxOffset(uint8_t offset);

bool configurePatternSequence(PatternSequence &patternSequence,
                              bool repeat = true,
                              uint8_t patternNumPerTriggerOut2 = 1);
bool sendPatternDisplayLUT(PatternSequence &patternSequence);

}; // namespace DLPC350

#endif