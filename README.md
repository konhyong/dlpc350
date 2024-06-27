# dlpc350

dlpc350 is a C++ based cross-platform library that provides an API to control multiple DLPC350 devices via USB.

It is based on Texas Instrument's DLP® LightCrafter™ 4500 and cryeo's [lightcrafter library](https://github.com/cryeo/lightcrafter).

API was re-implemented to allow easier usage by labs that utilize multiple projectors at once.

DLP® and LightCrafter™ are registered trademarks of Texas Instruments.

## Dependencies

This library uses the [hidapi library](https://github.com/libusb/hidapi)

## How to use

Currently this library is designed to be used as a submodule of a bigger CMake project.

## TODO

Add capability to install the library files

Add more functionality to the API

## References
- [DLPC350 Programmer's Guide](http://ti.com/lit/ug/dlpu010e/dlpu010e.pdf)