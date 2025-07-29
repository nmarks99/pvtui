# PVTUI

[![License](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Doxygen Documentation](https://img.shields.io/badge/docs-doxygen-blue)](https://nmarks99.github.io/pvtui/doc/doxygen/html/index.html)

PVTUI is a library for creating terminal/text based user interfaces for EPICS built on [FTXUI](https://github.com/ArthurSonzogni/FTXUI)
and [pvAccessCPP](https://github.com/epics-base/pvAccessCPP).

Various ready-to-use applications are included in this respository. The source code for
the following displays are located in the apps/ directory:
- asyn record
- motor record
- calcout record
- APS storage ring status

## Installation

Currently PVTUI is only officially supported on Linux. It has been tested on 
Red Hat Enterprise Linux 9 and Arch Linux.

### Dependencies
* [FTXUI](https://github.com/ArthurSonzogni/FTXUI)
* [EPICS Base](https://docs.epics-controls.org/en/latest/getting-started/installation.html)

### Building

```bash
mkdir build && cd build

# To clone and build FTXUI with this project (using FetchContent):
cmake .. -DEPICS_BASE=/path/to/epics/base -DFETCH_FTXUI=ON

# If FTXUI is installed on your system somewhere cmake can find it (with find_package):
cmake .. -DEPICS_BASE=/path/to/epics/base

make
```
