#ifndef DLPC350_USB_HPP
#define DLPC350_USB_HPP

#include "hidapi.h"
#include <cstdint>
#include <memory>
#include <vector>

namespace DLPC350 {
namespace USB {
/// @brief uint8_t array managed by unique pointers
using Buffer = std::unique_ptr<uint8_t, std::default_delete<uint8_t[]>>;

const uint16_t vendorId = 0x0451;
const uint16_t productId = 0x6401;
const int32_t readTimeout = 2000;

extern hid_device *device;
extern std::vector<hid_device *> devices;

// In/Out buffers equal to HID endpoint size + 1
// First byte is for Windows internal use and it is always 0
constexpr size_t packetSize = 64;
constexpr size_t bufferSize = packetSize + 1;

extern bool init();
extern bool exit();

extern bool open();
extern void close();

extern bool isConnected();
extern unsigned int deviceNum();
extern bool select(unsigned int index);
extern void printDevices();

extern Buffer read();
extern int32_t write(Buffer &data);
}; // namespace USB
}; // namespace DLPC350

#endif