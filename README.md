# PVTUI

[![License](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Doxygen Documentation](https://img.shields.io/badge/docs-doxygen-blue)](https://nmarks99.github.io/pvtui/doc/doxygen/html/index.html)

PVTUI is a library for creating terminal/text based user interfaces for EPICS built on [FTXUI](https://github.com/ArthurSonzogni/FTXUI)
and [pvAccessCPP](https://github.com/epics-base/pvAccessCPP).

![pvtui_motor](https://github.com/user-attachments/assets/346f036d-8ae0-4e19-ac58-93643c132bb7)

There are several ready-to-use applications included in this respository:
- motor record
- calcout record
- asyn record
- APS storage ring status

## Installation

Currently PVTUI is only officially supported on Linux. It has been tested on 
Red Hat Enterprise Linux 9 and Arch Linux. Cmake is used for building the project.

### Dependencies
* [FTXUI](https://github.com/ArthurSonzogni/FTXUI)
    - FTXUI can be automatically cloned and compiled when building PVTUI via cmake's FetchContent,
    or you can compile it yourself and install it somewhere that cmake can find it.
* [EPICS Base](https://docs.epics-controls.org/en/latest/getting-started/installation.html)
    - EPICS base must be installed on your system before building PVTUI.
    Make sure `EPICS_BASE` and `EPICS_HOST_ARCH` environment variables are set
    - EPICS base builds for MACOS/darwin targets must have c++11 features enabled.
        -  % make CXXFLAGS='-std=c++11'


### Building
Clone PVTUI from GitHub and build with cmake:

```bash
git clone https://github.com/nmarks99/pvtui.git
cd pvtui
mkdir build && cd build
cmake ..
make
```

There are several optional flags when running cmake:
- `-DEPICS_BASE`: (Default from environment) The path to EPICS base installation
- `-DFETCH_FTXUI`[ON/OFF]: (Default ON) Whether or not to clone and compile FTXUI
- `-DBUILD_APPS`[ON/OFF]: (Default ON) Whether or not to build applications in apps/ directory
- `-DBUILD_TESTS`[ON/OFF]: (Default OFF) Whether or not to build tests in tests/ directory
- `-DBUILD_DOCS`[ON/OFF]: (Default OFF) Whether or not to build Doxygen documentation


## Contributing
If you create a new PVTUI app which could be useful to others, fork this repo,
add your app to the apps/ directory, and submit a pull request.
