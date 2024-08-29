#include "multi350/usb.hpp"
#include <iostream>

namespace MULTI350 {
namespace USB {

hid_device *device = nullptr;
std::vector<hid_device *> devices;

inline bool init() { return (hid_init() == 0); }

inline bool exit() { return (hid_exit() == 0); }

inline bool open() {
  if (!devices.empty())
    close();

  hid_device_info *hid_info;
  hid_info = hid_enumerate(vendorId, productId);
  if (!hid_info) {
    return false;
  }

  while (hid_info) {
    if (hid_info->interface_number == 0) {
      device = hid_open_path(hid_info->path);

      if (!device) {
        std::wcerr << "[HID] Failed to open device: " << hid_info->serial_number
                   << std::endl;
        close();
        return false;
      }

      devices.push_back(device);
    }
    hid_info = hid_info->next;
    device = nullptr; // reset to default
  }

  return true;
}

inline void close() {
  for (auto *handle : devices) {
    hid_close(handle);
  }
  devices.clear();
  device = nullptr;
}

inline bool isConnected() { return !devices.empty(); }

inline unsigned int deviceNum() { return devices.size(); }

inline bool select(unsigned int index) {
  if (index >= devices.size()) {
    std::cerr << "Unable to select device " << index << std::endl;
    return false;
  }

  device = devices[index];
  return true;
}

inline void printDevices() {
  struct hid_device_info *hid_info;
  hid_info = hid_enumerate(vendorId, productId);
  std::cout << "[Device List]" << std::endl;
  while (hid_info) {
    if (hid_info->interface_number == 0) {
      std::cout << " path: " << hid_info->path << std::endl;
      // std::wcout << "  manufacturer: " << hid_info->manufacturer_string
      // << std::endl;
      // std::wcout << "  product: " << hid_info->product_string << std::endl;
      // std::wcout << "  S/N: " << hid_info->serial_number << std::endl;
    }
    hid_info = hid_info->next;
  }
}

inline Buffer read() {
  if (!isConnected())
    return nullptr;

  if (!device) {
    std::cerr << "Device not selected" << std::endl;
    return nullptr;
  }

  Buffer ret(new uint8_t[bufferSize]);
  int32_t readBytes =
      hid_read_timeout(device, ret.get(), bufferSize, readTimeout);

  if (readBytes == -1) {
    std::cerr << "USB Read failed" << std::endl;
    close();
    return nullptr;
  }

  return ret;
}

inline int32_t write(Buffer &data) {
  if (!isConnected())
    return -1;

  if (!device) {
    std::cerr << "Device not selected" << std::endl;
    return -1;
  }

  int32_t writtenBytes = hid_write(device, data.get(), bufferSize);

  if (writtenBytes == -1) {
    std::cerr << "USB Write failed" << std::endl;
    close();
    return -1;
  }

  return writtenBytes;
}
}; // namespace USB
}; // namespace MULTI350
