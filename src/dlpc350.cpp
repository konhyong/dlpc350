#include "dlpc350/dlpc350.hpp"
#include "dlpc350/message.hpp"

namespace DLPC350 {
/**
 * getHardwareStatus
 * CMD2 : 0x1A, CMD3 : 0x0A
 */
std::unique_ptr<HardwareStatus> getHardwareStatus() {
  auto result = sendGetMessage<HardwareStatus>(0x1A0A);
  return std::make_unique<HardwareStatus>(*result.get());
}

/**
 * getSystemStatus
 * CMD2 : 0x1A, CMD3 : 0x0B
 */
std::unique_ptr<SystemStatus> getSystemStatus() {
  auto result = sendGetMessage<SystemStatus>(0x1A0B);
  return std::make_unique<SystemStatus>(*result.get());
}

/**
 * getMainStatus
 * CMD2 : 0x1A, CMD3 : 0x0C
 */
std::unique_ptr<MainStatus> getMainStatus() {
  auto result = sendGetMessage<MainStatus>(0x1A0C);
  return std::make_unique<MainStatus>(*result.get());
}

/**
 * getVersion
 * CMD2 : 0x02, CMD3 : 0x05
 */
std::unique_ptr<Version> getVersion() {
  auto result = sendGetMessage<uint32_t>(0x0205);
  return std::make_unique<Version>(*(result.get()), *(result.get() + 1),
                                   *(result.get() + 2), *(result.get() + 3));
}

/**
 * getFirmwareTag
 * CMD2 : 0x1A, CMD3 : 0xFF
 */
std::unique_ptr<std::string> getFirmwareTag() {
  auto result = sendGetMessage<char>(0x1AFF);
  return std::make_unique<std::string>(result.get());
}

/**
 * softwareReset
 * CMD2 : 0x08, CMD3 : 0x02
 */
bool softwareReset() {
  auto result = sendNoAckMessage(0x0802);
  return (result > 0);
}

/**
 * getPowerMode
 * CMD2 : 0x02, CMD3 : 0x00
 */
std::unique_ptr<PowerMode> getPowerMode() {
  auto result = sendGetMessage<PowerMode>(0x0200);
  return std::make_unique<PowerMode>(*result.get());
}

/**
 * setPowerMode
 * CMD2 : 0x02, CMD3 : 0x00, Param : 1
 */
bool setPowerMode(PowerMode mode) {
  auto result = sendSetMessage<uint8_t>(0x0200, static_cast<uint8_t>(mode));
  return (result != nullptr);
}

/**
 * getColorCurtain
 * CMD2 : 0x11, CMD3 : 0x00
 */
std::unique_ptr<CurtainColor> getColorCurtain() {
  auto result = sendGetMessage<uint16_t>(0x1100);
  return std::make_unique<CurtainColor>(*result.get(), *(result.get() + 1),
                                        *(result.get() + 2));
}

/**
 * setColorCurtain
 * CMD2 : 0x11, CMD3 : 0x00, Param : 3
 */
bool setColorCurtain(uint16_t red, uint16_t green, uint16_t blue) {
  auto result = sendSetMessage<uint16_t, uint16_t, uint16_t>(
      0x1100, std::forward<uint16_t>(red), std::forward<uint16_t>(green),
      std::forward<uint16_t>(blue));
  return (result != nullptr);
}

/**
 * getInputSource
 * CMD2 : 0x1A, CMD3 : 0x00
 */
std::unique_ptr<InputSource> getInputSource() {
  auto result = sendGetMessage<InputSource>(0x1A00);
  return std::make_unique<InputSource>(*result.get());
}

/**
 * setInputSource
 * CMD2 : 0x1A, CMD3 : 0x00, Param : 1
 */
bool setInputSource(InputType type, InputBitDepth bitDepth) {
  auto result =
      sendSetMessage<uint8_t>(0x1A00, InputSource(type, bitDepth).value);
  return (result != nullptr);
}

/**
 * getTestPattern
 * CMD2 : 0x12, CMD3 : 0x03
 */
std::unique_ptr<TestPattern> getTestPattern() {
  assert(getInputSource()->type == InputType::TEST_PATTERN);
  auto result = sendGetMessage<TestPattern>(0x1203);
  return std::make_unique<TestPattern>(*result.get());
}

/**
 * setTestPattern
 * CMD2 : 0x12, CMD3 : 0x03, Param : 1
 */
bool setTestPattern(TestPattern pattern) {
  assert(getInputSource()->type == InputType::TEST_PATTERN);
  auto result = sendSetMessage<uint8_t>(0x1203, static_cast<uint8_t>(pattern));
  return (result != nullptr);
}

/**
 * getLEDEnable
 * CMD2 : 0x1A, CMD3 : 0x07
 */
std::unique_ptr<LEDEnable> getLEDEnable() {
  auto result = sendGetMessage<LEDEnable>(0x1A07);
  return std::make_unique<LEDEnable>(*result.get());
}

/**
 * setLEDEnable
 * CMD2 : 0x1A, CMD3 : 0x07, Param : 1
 */
bool setLEDEnable(LEDEnableMode mode, bool redEnabled, bool greenEnabled,
                  bool blueEnabled) {
  auto result = sendSetMessage<uint8_t>(
      0x1A07, LEDEnable(mode, redEnabled, greenEnabled, blueEnabled).value);
  return (result != nullptr);
}

/**
 * getLEDCurrent
 * CMD2 : 0x0B, CMD3 : 0x01
 */
std::unique_ptr<LEDCurrent> getLEDCurrent() {
  auto result = sendGetMessage<uint32_t>(0x0B01);
  return std::make_unique<LEDCurrent>(*result.get());
}

/**
 * setLEDCurrent
 * CMD2 : 0x0B, CMD3 : 0x01, Param : 3
 */
bool setLEDCurrent(uint8_t red, uint8_t green, uint8_t blue) {
  auto result = sendSetMessage<uint8_t, uint8_t, uint8_t>(
      0x0B01, 255 - red, 255 - green, 255 - blue);
  return (result != nullptr);
}

/**
 * getDisplayMode
 * CMD2 : 0x1A, CMD3 : 0x1B
 */
std::unique_ptr<DisplayMode> getDisplayMode() {
  auto result = sendGetMessage<DisplayMode>(0x1A1B);
  return std::make_unique<DisplayMode>(*result.get());
}

/**
 * setDisplayMode
 * CMD2 : 0x1A, CMD3 : 0x1B, Param : 1
 */
bool setDisplayMode(DisplayMode mode) {
  auto result = sendSetMessage<uint8_t>(0x1A1B, static_cast<uint8_t>(mode));
  return (result != nullptr);
}

/**
 * getGammaCorrection
 * CMD2 : 0x1A, CMD3 : 0x0E
 */
std::unique_ptr<GammaCorrection> getGammaCorrection() {
  auto result = sendGetMessage<GammaCorrection>(0x1A0E);
  return std::make_unique<GammaCorrection>(*result.get());
}

/**
 * setGammaCorrection
 * CMD2 : 0x1A, CMD3 : 0x0E, Param : 1
 */
bool setGammaCorrection(bool enable, bool degammaTable) {
  auto result = sendSetMessage<uint8_t>(
      0x1A0E, GammaCorrection(degammaTable, enable).value);
  return (result != nullptr);
}

/**
 * startPatternValidation
 * CMD2 : 0x1A, CMD3 : 0x1A, Param : 1 // dummy byte
 */
std::unique_ptr<PatternSequenceValidation> startPatternValidation() {
  auto result = sendSetMessage<PatternSequenceValidation>(0x1A1A, 0x00);
  return std::make_unique<PatternSequenceValidation>(*result.get());
}

/**
 * checkPatternValidation
 * CMD2 : 0x1A, CMD3 : 0x1A
 */
std::unique_ptr<PatternSequenceValidation> checkPatternValidation() {
  auto result = sendGetMessage<PatternSequenceValidation>(0x1A1A);
  return std::make_unique<PatternSequenceValidation>(*result.get());
}

/**
 * getPatternTriggerMode
 * CMD2 : 0x1A, CMD3 : 0x23
 */
std::unique_ptr<PatternTriggerMode> getPatternTriggerMode() {
  auto result = sendGetMessage<PatternTriggerMode>(0x1A23);
  return std::make_unique<PatternTriggerMode>(*result.get());
}

/**
 * setPatternTriggerMode
 * CMD2 : 0x1A, CMD3 : 0x23, Param : 1
 */
bool setPatternTriggerMode(PatternTriggerMode mode) {
  auto result = sendSetMessage<uint8_t>(0x1A23, static_cast<uint8_t>(mode));
  return (result != nullptr);
}

/**
 * getPatternDataSource
 * CMD2 : 0x1A, CMD3 : 0x22
 */
std::unique_ptr<PatternDataSource> getPatternDataSource() {
  auto result = sendGetMessage<PatternDataSource>(0x1A22);
  return std::make_unique<PatternDataSource>(*result.get());
}

/**
 * setPatternDataSource
 * CMD2 : 0x1A, CMD3 : 0x22, Param : 1
 */
bool setPatternDataSource(PatternDataSource input) {
  auto result = sendSetMessage<uint8_t>(0x1A22, static_cast<uint8_t>(input));
  return (result != nullptr);
}

/**
 * getPatternStatus
 * CMD2 : 0x1A, CMD3 : 0x24
 */
std::unique_ptr<PatternStatus> getPatternStatus() {
  auto result = sendGetMessage<PatternStatus>(0x1A24);
  return std::make_unique<PatternStatus>(*result.get());
}

/**
 * setPatternStatus
 * CMD2 : 0x1A, CMD3 : 0x24, Param : 1
 */
bool setPatternStatus(PatternStatus mode) {
  auto result = sendSetMessage<uint8_t>(0x1A24, static_cast<uint8_t>(mode));
  return (result != nullptr);
}

/**
 * getPatternPeriod
 * CMD2 : 0x1A, CMD3 : 0x29
 */
std::unique_ptr<PatternPeriod> getPatternPeriod() {
  auto result = sendGetMessage<uint32_t>(0x1A29);
  return std::make_unique<PatternPeriod>(*result.get(), *(result.get() + 1));
}

/**
 * setPatternPeriod
 * CMD2 : 0x1A, CMD3 : 0x29, Param : 1
 */
bool setPatternPeriod(uint32_t exposure, uint32_t frame) {
  assert(exposure <= frame);
  assert(frame - exposure > 230);

  auto result = sendSetMessage<uint32_t, uint32_t>(
      0x1A29, std::forward<uint32_t>(exposure), std::forward<uint32_t>(frame));
  return (result != nullptr);
}

/**
 * setMailboxMode
 * CMD2 : 0x1A, CMD3 : 0x33, Param : 1
 */
bool setMailboxMode(MailboxMode mode) {
  auto result = sendSetMessage<uint8_t>(0x1A33, static_cast<uint8_t>(mode));
  return (result != nullptr);
}

/**
 * setMailboxOffset
 * CMD2 : 0x1A, CMD3 : 0x32
 */
bool setMailboxOffset(uint8_t offset) {
  assert(offset <= 127);

  auto result = sendSetMessage<uint8_t>(0x1A32, std::forward<uint8_t>(offset));
  return (result != nullptr);
}

/**
 * configurePatternSequence
 * CMD2 : 0x1A, CMD3 : 0x31, Param : 4
 */
bool configurePatternSequence(PatternSequence &patternSequence, bool repeat,
                              uint8_t patternNumPerTriggerOut2) {
  if (repeat) {
    patternNumPerTriggerOut2 =
        static_cast<uint8_t>(patternSequence.sizePattern());
  }
  auto result = sendSetMessage<uint8_t, uint8_t, uint8_t, uint8_t>(
      0x1A31, static_cast<uint8_t>(patternSequence.sizePattern() - 1),
      static_cast<uint8_t>(repeat),
      static_cast<uint8_t>(patternNumPerTriggerOut2 - 1),
      static_cast<uint8_t>(0)); // Irrelevant unless PatternDataSource::INTERNAL
  return (result != nullptr);
}

/**
 * sendPatternDisplayLUT
 * CMD2 : 0x1A, CMD3 : 0x34
 * @param
 *
 * @return
 *
 */
bool sendPatternDisplayLUT(PatternSequence &patternSequence) {
  if (!setMailboxMode(MailboxMode::PATTERN))
    return false;

  setMailboxOffset(0);

  auto send = Message(Message::Type::WRITE, 0x1A34);

  // TODO: possible to use sendSetMessage & addData?
  for (size_t i = 0; i < patternSequence.sizePattern(); i++) {
    Pattern &pattern = patternSequence.getPattern(i);
    uint8_t *value = reinterpret_cast<uint8_t *>(&pattern.value);
    for (size_t j = 0; j < 3; j++) {
      send.data[send.length++] = *(value++);
    }
  }

  auto result = transact(send);

  setMailboxMode(MailboxMode::DISABLE);

  return (result != nullptr);
}

}; // namespace DLPC350