cmake_minimum_required (VERSION 3.22)

project(pvtui
  LANGUAGES CXX
  VERSION 1.0.0
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- Fetch FTXUI --------------------------------------------------------------
include(FetchContent)

set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
set(FETCHCONTENT_QUIET OFF)

FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/arthursonzogni/ftxui.git
  GIT_TAG        v6.0.2
  GIT_PROGRESS   TRUE
  GIT_SHALLOW    TRUE
  EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(ftxui)
# ------------------------------------------------------------------------------

# --- EPICS Base ---------------------------------------------------------------
set(EPICS_BASE /APSshare/epics/base-7.0.8)
link_directories(${EPICS_BASE}/lib/rhel9-x86_64)

set(EPICS_BASE_LIBRARIES
    ${EPICS_BASE}/lib/rhel9-x86_64/libpvAccessCA.so
    ${EPICS_BASE}/lib/rhel9-x86_64/libca.so
    ${EPICS_BASE}/lib/rhel9-x86_64/libpvAccess.so
    ${EPICS_BASE}/lib/rhel9-x86_64/libpvData.so
    ${EPICS_BASE}/lib/rhel9-x86_64/libCom.so
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
add_executable(pvtui_motor src/motor.cpp src/pvtui.cpp src/pvgroup.cpp)
target_include_directories(pvtui_motor PUBLIC src ${EPICS_BASE_INCLUDES})

target_link_libraries(pvtui_motor
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
