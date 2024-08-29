#include "multi350/status.hpp"

namespace MULTI350 {
namespace Status {
namespace Hardware {
std::vector<std::string> initError = {"Error", "Successful"};
std::vector<std::string> DRCError = {
    "No error has occurred", "Multiple overlapping bias or reset operations "
                             "are accessing the same DMD block"};
std::vector<std::string> forcedSwap = {"No error has occurred",
                                       "Forced Swap Error occurred"};
std::vector<std::string> sequenceAbort = {
    "No error has occurred",
    "Sequencer has detected an error condition that caused an abort"};
std::vector<std::string> sequenceError = {"No error has occurred",
                                          "Sequencer detected an error"};
}; // namespace Hardware

namespace System {
std::vector<std::string> memoryTest = {"Internal Memory Test failed",
                                       "Internal Memory Test passed"};
}; // namespace System

namespace Main {
std::vector<std::string> DMDParked = {"DMD micromirrors are not parked",
                                      "DMD micromirrors are parked"};
std::vector<std::string> sequenceRunning = {"Sequencer is stopped",
                                            "Sequencer is running normally"};
std::vector<std::string> bufferFrozen = {"Frame buffer is not frozen",
                                         "Frame buffer is frozen"};
std::vector<std::string> gammaCorrection = {"Gamma correction is disabled",
                                            "Gamma correction is enabled"};
}; // namespace Main
}; // namespace Status
}; // namespace MULTI350