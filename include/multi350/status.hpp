#ifndef MULTI350_STATUS_HPP
#define MULTI350_STATUS_HPP

#include <string>
#include <vector>

namespace multi350 {
namespace status {
namespace hardware {
extern std::vector<std::string> initError;
extern std::vector<std::string> DRCError;
extern std::vector<std::string> forcedSwap;
extern std::vector<std::string> sequenceAbort;
extern std::vector<std::string> sequenceError;
}; // namespace hardware

namespace system {
extern std::vector<std::string> memoryTest;
}; // namespace system

namespace main {
extern std::vector<std::string> DMDParked;
extern std::vector<std::string> sequenceRunning;
extern std::vector<std::string> bufferFrozen;
extern std::vector<std::string> gammaCorrection;
}; // namespace main
}; // namespace status
}; // namespace multi350

#endif