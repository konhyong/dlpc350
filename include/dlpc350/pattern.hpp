#ifndef DLPC350_PATTERN_HPP
#define DLPC350_PATTERN_HPP

#include "usb.hpp"
#include <cassert>
#include <cstdint>
#include <vector>

namespace DLPC350 {

const size_t maxPatternInSequence = 128;

struct Pattern {
  enum class LEDSelect : uint8_t {
    PASS = 0, // No LED, Pass through
    RED = 1,
    GREEN = 2,
    YELLOW = 3, // Green + Red
    BLUE = 4,
    MAGENTA = 5, // Blue + Red
    CYAN = 6,    // Blue + Green
    WHITE = 7    // Red + Blue + Green
  };

  enum class TriggerType : uint8_t {
    INTERNAL = 0,
    EXTERNAL_POSITIVE = 1,
    EXTERNAL_NEGATIVE = 2,
    NO_TRIGGER = 3 // Continue from previous, full exposure
  };

  enum class Pattern1bit : uint8_t {
    G0 = 0,
    G1 = 1,
    G2 = 2,
    G3 = 3,
    G4 = 4,
    G5 = 5,
    G6 = 6,
    G7 = 7,
    R0 = 8,
    R1 = 9,
    R2 = 10,
    R3 = 11,
    R4 = 12,
    R5 = 13,
    R6 = 14,
    R7 = 15,
    B0 = 16,
    B1 = 17,
    B2 = 18,
    B3 = 19,
    B4 = 20,
    B5 = 21,
    B6 = 22,
    B7 = 23,
    BLACK = 24
  };

  enum class Pattern2bit : uint8_t {
    G1G0 = 0,
    G3G2 = 1,
    G5G4 = 2,
    G7G6 = 3,
    R1R0 = 4,
    R3R2 = 5,
    R5R4 = 6,
    R7R6 = 7,
    B1B0 = 8,
    B3B2 = 9,
    B5B4 = 10,
    B7B6 = 11
  };

  enum class Pattern3bit : uint8_t {
    G2G1G0 = 0,
    G5G4G3 = 1,
    R0G7G6 = 2,
    R3R2R1 = 3,
    R6R5R4 = 4,
    B1B0R7 = 5,
    B4B3B2 = 6,
    B7B6B5 = 7
  };

  enum class Pattern4bit : uint8_t {
    G3G2G1G0 = 0,
    G7G6G5G4 = 1,
    R3R2R1R0 = 2,
    R7R6R5R4 = 3,
    B3B2B1B0 = 4,
    B7B6B5B4 = 5
  };

  enum class Pattern5bit : uint8_t {
    G5G4G3G2G1 = 0,
    R3R2R1R0G7 = 1,
    B1B0R7R6R5 = 2,
    B7B6B5B4B3 = 3
  };

  enum class Pattern6bit : uint8_t {
    G5G4G3G2G1G0 = 0,
    R3R2R1R0G7G6 = 1,
    B1B0R7R6R5R4 = 2,
    B7B6B5B4B3B2 = 3
  };

  enum class Pattern7bit : uint8_t {
    G7G6G5G4G3G2G1 = 0,
    R7R6R5R4R3R2R1 = 1,
    B7B6B5B4B3B2B1 = 2
  };

  enum class Pattern8bit : uint8_t {
    G7G6G5G4G3G2G1G0 = 0,
    R7R6R5R4R3R2R1R0 = 1,
    B7B6B5B4B3B2B1B0 = 2
  };

  union {
    uint32_t value;
    struct {
      TriggerType triggerType : 2;
      uint8_t patternNumber : 6;
      uint8_t bitDepth : 4;
      LEDSelect ledSelect : 4;
      bool invertPattern : 1;
      bool insertBlack : 1;
      bool bufferSwap : 1; // requires 230us before next pattern
      bool triggerOutPrevious : 1;
      uint8_t : 4;
    };
  };

  Pattern() : value{0} {}
  Pattern(TriggerType _triggerType, uint8_t _patternNumber, uint8_t _bitDepth,
          LEDSelect _ledSelect, bool _invertPattern, bool _insertBlack,
          bool _bufferSwap, bool _triggerOutPrevious)
      : triggerType{_triggerType}, patternNumber{_patternNumber},
        bitDepth{_bitDepth}, ledSelect{_ledSelect},
        invertPattern{_invertPattern}, insertBlack{_insertBlack},
        bufferSwap{_bufferSwap}, triggerOutPrevious{_triggerOutPrevious} {}
};

class PatternSequence {
public:
  PatternSequence() : patternNum(0), exposure{0x4010}, period{0x411A} {}

  void clear() { patternNum = 0; }

  template <typename PatternType>
  bool addPattern(Pattern::TriggerType triggerType, PatternType patternType,
                  uint8_t bitDepth, Pattern::LEDSelect ledSelect,
                  bool invertPattern = false, bool insertBlack = false,
                  bool triggerOutPrevious = false) {

    uint8_t patternNumber = static_cast<uint8_t>(patternType);

    assert(patternNumber <= 24);
    assert(bitDepth <= 8);

    bool bufferSwap = (sizePattern() == 0) ? true : false;

    Pattern pattern(triggerType, patternNumber, bitDepth, ledSelect,
                    invertPattern, insertBlack, bufferSwap, triggerOutPrevious);

    addPattern(pattern);

    return true;
  }

  void addPattern(Pattern &pat) { pattern[patternNum++] = pat; }

  inline size_t sizePattern() { return patternNum; }

  inline Pattern &getPattern(size_t index) { return pattern[index]; }

  void setExposure(uint32_t _exposure) { exposure = _exposure; }
  inline uint32_t getExposure() { return exposure; }

  void setPeriod(uint32_t _period) { period = _period; }
  inline uint32_t getPeriod() { return period; }

private:
  size_t patternNum;
  Pattern pattern[maxPatternInSequence];
  uint32_t exposure;
  uint32_t period;
};
}; // namespace DLPC350

#endif