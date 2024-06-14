#ifndef DLPC350_STATUS_HPP
#define DLPC350_STATUS_HPP

#include <string>
#include <vector>

namespace DLPC350 {
namespace Status {
namespace Hardware {
extern std::vector<std::string> initError;
extern std::vector<std::string> DRCError;
extern std::vector<std::string> forcedSwap;
extern std::vector<std::string> sequenceAbort;
extern std::vector<std::string> sequenceError;
}; // namespace Hardware

namespace System {
extern std::vector<std::string> memoryTest;
}; // namespace System

namespace Main {
extern std::vector<std::string> DMDParked;
extern std::vector<std::string> sequenceRunning;
extern std::vector<std::string> bufferFrozen;
extern std::vector<std::string> gammaCorrection;
}; // namespace Main
}; // namespace Status
}; // namespace DLPC350

#endif