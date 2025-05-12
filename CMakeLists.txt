cmake_minimum_required (VERSION 3.22)

project(pvtui
  LANGUAGES CXX
  VERSION 1.0.0
)

# Generate compile_commands.json for LSP's
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# C/C++ Standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Local cmake projects might be built here
list(APPEND CMAKE_PREFIX_PATH "$ENV{HOME}/.local")

# Built binaries in build/bin, libraries in build/lib
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# --- Fetch FTXUI --------------------------------------------------------------
find_package(ftxui REQUIRED CONFIG)
if(ftxui_FOUND)
    message(STATUS "ftxui found!")
    message(STATUS "ftxui DIR: ${ftxui_DIR}")
endif()
# include(FetchContent)
#
# set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
# set(FETCHCONTENT_QUIET OFF)
#
# FetchContent_Declare(ftxui
  # GIT_REPOSITORY https://github.com/arthursonzogni/ftxui.git
  # GIT_TAG        v6.0.2
  # GIT_PROGRESS   TRUE
  # GIT_SHALLOW    TRUE
  # EXCLUDE_FROM_ALL
# )
# FetchContent_MakeAvailable(ftxui)
# ------------------------------------------------------------------------------


# --- EPICS Base ---------------------------------------------------------------
# Get path to EPICS base
set(EPICS_BASE "" CACHE STRING "Path to EPICS base")
if (NOT EPICS_BASE)
  message(FATAL_ERROR "Missing required variable 'EPICS_BASE'")
endif()
if(NOT IS_DIRECTORY "${EPICS_BASE}")
  message(FATAL_ERROR "Specified EPICS_BASE does not exist: ${EPICS_BASE}")
endif()
message(STATUS "Found EPICS_BASE: ${EPICS_BASE}")

# Get EPICS host architecture
set(EPICS_HOST_ARCH "$ENV{EPICS_HOST_ARCH}" CACHE STRING "Host architecture (e.g. linux-x86_64")
if (NOT EPICS_HOST_ARCH)
  message(FATAL_ERROR "'EPICS_HOST_ARCH' not defined")
endif()
message(STATUS "EPICS_HOST_ARCH: ${EPICS_HOST_ARCH}")

link_directories(${EPICS_BASE}/lib/${EPICS_HOST_ARCH})

set(EPICS_BASE_LIBRARIES
    "${EPICS_BASE}/lib/${EPICS_HOST_ARCH}/libpvAccessCA.so"
    "${EPICS_BASE}/lib/${EPICS_HOST_ARCH}/libca.so"
    "${EPICS_BASE}/lib/${EPICS_HOST_ARCH}/libpvAccess.so"
    "${EPICS_BASE}/lib/${EPICS_HOST_ARCH}/libpvData.so"
    "${EPICS_BASE}/lib/${EPICS_HOST_ARCH}/libCom.so"
)

set(EPICS_BASE_INCLUDES
    ${EPICS_BASE}/include
    ${EPICS_BASE}/include/compiler/gcc
    ${EPICS_BASE}/include/pv
    ${EPICS_BASE}/include/pva
    ${EPICS_BASE}/include/os/Linux
)
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
add_executable(pvtui_motor apps/motor.cpp src/pvtui.cpp src/pvgroup.cpp)
target_include_directories(pvtui_motor PUBLIC src ${EPICS_BASE_INCLUDES})

target_link_libraries(pvtui_motor
  PUBLIC ${EPICS_BASE_LIBRARIES}
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component
)
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
add_executable(pvtui_calcout apps/calcout.cpp src/pvtui.cpp src/pvgroup.cpp)
target_include_directories(pvtui_calcout PUBLIC src ${EPICS_BASE_INCLUDES})

target_link_libraries(pvtui_calcout
  PUBLIC ${EPICS_BASE_LIBRARIES}
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component
)
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
add_executable(pvtui_sr apps/sr.cpp src/pvtui.cpp src/pvgroup.cpp)
target_include_directories(pvtui_sr PUBLIC src ${EPICS_BASE_INCLUDES})

target_link_libraries(pvtui_sr
  PUBLIC ${EPICS_BASE_LIBRARIES}
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component
)
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
add_executable(test tests/test.cpp src/pvgroup.cpp)
target_include_directories(test PUBLIC tests src ${EPICS_BASE_INCLUDES})

target_link_libraries(test
  PUBLIC ${EPICS_BASE_LIBRARIES}
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component
)
# ------------------------------------------------------------------------------
