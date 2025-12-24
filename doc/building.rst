.. _building:

Building from Source
====================
At this time no binaries for PVTUI are provided so you must build it from source. However,
there are not many dependencies so building from source is quite straightforward.


Dependencies
------------

* C++17 compiler (e.g. GCC 8+)
* `CMake <https://cmake.org>`_
* `FTXUI <https://github.com/ArthurSonzogni/FTXUI>`_
* `EPICS Base <https://epics-controls.org/resources-and-support/base/>`_


Building
--------

Clone PVTUI from GitHub and build with cmake ::

    git clone https://github.com/nmarks99/pvtui.git
    cd pvtui
    mkdir build && cd build
    cmake ..
    make

There are several optional flags when running cmake

* ``-DEPICS_BASE``: (Default from environment) The path to EPICS base installation
* ``-DFETCH_FTXUI``: (Default ON) Whether or not to clone and compile FTXUI
* ``-DBUILD_APPS``: (Default ON) Whether or not to build applications in apps/ directory
* ``-DBUILD_TESTS``: (Default OFF) Whether or not to build tests in tests/ directory
* ``-DBUILD_DOCS``: (Default OFF) Whether or not to build Doxygen documentation
