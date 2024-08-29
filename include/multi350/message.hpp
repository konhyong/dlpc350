#ifndef MULTI350_MESSAGE_HPP
#define MULTI350_MESSAGE_HPP

#include "usb.hpp"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <utility>

namespace MULTI350 {

namespace Internal {
constexpr size_t maxMessageDataSize = 512;
constexpr bool verbose = false;
}; // namespace Internal

struct Message {
  enum class Type : bool { WRITE = 0, READ = 1 };
  Message() : flags{0, 0, 0, 1, Type::WRITE}, sequence{0}, length{0}, data{0} {}

  template <typename... ParamList>
  Message(Type _type, uint16_t cmd, ParamList &&...params)
      : flags{0, 0, 0, 1, _type}, sequence{0}, length{2}, data{0} {
    command = cmd;
    if (sizeof...(params) > 0) {
      addData(std::forward<ParamList>(params)...);
    }
  }

  inline void addData() {}

  template <typename T> inline void addData(T first) {
    T *ptr = reinterpret_cast<T *>(&data[length]);
    *ptr = first;
    length += sizeof(first);
  }

  template <typename T, typename... ParamList>
  inline void addData(T first, ParamList &&...params) {
    addData(first);
    if (sizeof...(params) > 0) {
      addData(std::forward<ParamList>(params)...);
    }
  }

  struct {
    uint8_t destination : 3;
    uint8_t reserved : 2;
    bool error : 1;
    bool reply : 1;
    Type rw : 1;
  } flags;

  uint8_t sequence;
  uint16_t length;

  union {
    uint16_t command;
    uint8_t data[Internal::maxMessageDataSize];
  };
};

extern inline std::unique_ptr<Message> read() {
  auto received = USB::read();

  if (received == nullptr) {
    std::cerr << "Message Read failed" << std::endl;
    return nullptr;
  }

  auto ret =
      std::unique_ptr<Message>(new Message(), std::default_delete<Message>());
  memcpy(ret.get(), received.get(), USB::bufferSize);

  return ret;
}

extern inline int32_t write(Message &msg) {
  uint16_t headerBytes =
      sizeof(msg.flags) + sizeof(msg.sequence) + sizeof(msg.length);
  uint16_t maxDataSize = USB::packetSize - headerBytes;
  uint16_t totalWrittenBytes = 0;
  uint16_t writtenBytes = std::min(msg.length, maxDataSize);

  USB::Buffer buffer(new uint8_t[USB::bufferSize]);
  memset(buffer.get(), 0, sizeof(uint8_t) * USB::bufferSize);
  memcpy(buffer.get() + 1, &msg,
         sizeof(uint8_t) * (headerBytes + writtenBytes));
  if (USB::write(buffer) == -1) {
    std::cerr << "Message write failed" << std::endl;
    return -1;
  }

  totalWrittenBytes += writtenBytes;

  while (totalWrittenBytes < msg.length) {
    writtenBytes =
        std::min(static_cast<uint16_t>(USB::packetSize),
                 static_cast<uint16_t>(msg.length - totalWrittenBytes));
    memcpy(buffer.get() + 1, &msg.data[totalWrittenBytes],
           sizeof(uint8_t) * writtenBytes);
    if (USB::write(buffer) == -1) {
      std::cerr << "Message write failed" << std::endl;
      return -1;
    }
    totalWrittenBytes += writtenBytes;
  }

  return totalWrittenBytes + headerBytes;
}

template <typename T>
using MessageData = std::unique_ptr<T, std::default_delete<T[]>>;

template <typename T = uint8_t> extern MessageData<T> transact(Message &msg) {
  int32_t result = write(msg);

  if (Internal::verbose) {
    std::cout << "W(" << msg.length << "): ";
    for (int i = 0; i < msg.length; ++i) {
      std::cout << std::hex << std::setw(4)
                << static_cast<unsigned int>(msg.data[i]) << " ";
    }
    std::cout << std::endl;
  }

  if (msg.flags.reply) {
    if (result <= 0) {
      std::cerr << "Failed to send message" << std::endl;
      return nullptr;
    }

    auto received = read();

    if (received == nullptr) {
      std::cerr << "Failed to receive proper reply" << std::endl;
      return nullptr;
    }
    if (received->flags.error ||
        (received->flags.rw == Message::Type::READ && received->length == 0)) {
      std::cerr << "Reply is empty/erroneous" << std::endl;
      return nullptr;
    }

    MessageData<T> ret(new T[USB::packetSize]);
    memcpy(ret.get(), received->data, USB::packetSize);

    if (Internal::verbose) {
      std::cout << "R(" << received->length << "): ";
      for (int i = 0; i < received->length; ++i) {
        std::cout << std::hex << std::setw(4)
                  << static_cast<unsigned int>(received->data[i]) << " ";
      }
      std::cout << std::endl;
    }

    return ret;
  }

  std::cerr << "Message set to no ack. Use transactNoAck(Msg)." << std::endl;

  return nullptr;
}

template <typename T = uint8_t>
extern inline MessageData<T> sendGetMessage(uint16_t cmd) {
  auto send = Message(Message::Type::READ, cmd);
  return transact<T>(send);
}

template <typename... ParamList>
extern inline MessageData<uint8_t> sendSetMessage(uint16_t cmd,
                                                  ParamList &&...params) {
  auto send =
      Message(Message::Type::WRITE, cmd, std::forward<ParamList>(params)...);
  return transact<uint8_t>(send);
}

template <typename... ParamList>
extern inline int32_t sendNoAckMessage(uint16_t cmd, ParamList &&...params) {
  auto send =
      Message(Message::Type::WRITE, cmd, std::forward<ParamList>(params)...);
  send.flags.reply = false;

  return write(send);
}
}; // namespace MULTI350

#endif