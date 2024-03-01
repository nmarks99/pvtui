cmake_minimum_required(VERSION 3.12)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(pv_client C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(EPICS_BASE /APSshare/epics/base-7.0.8)
link_directories(${EPICS_BASE}/lib/rhel9-x86_64)

set(COMMON_LIBRARIES
    ${EPICS_BASE}/lib/rhel9-x86_64/libca.so
    ${EPICS_BASE}/lib/rhel9-x86_64/libpvAccess.so
    ${EPICS_BASE}/lib/rhel9-x86_64/libpvData.so
)

set(COMMON_INCLUDES
    ${EPICS_BASE}/include
    ${EPICS_BASE}/include/compiler/gcc
    ${EPICS_BASE}/include/pv
    ${EPICS_BASE}/include/pva
    ${EPICS_BASE}/include/os/Linux
)

add_executable(pv_client ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp)
target_include_directories(pv_client PUBLIC ${COMMON_INCLUDES})
target_link_libraries(pv_client PUBLIC ${COMMON_LIBRARIES})

add_executable(example ${CMAKE_CURRENT_LIST_DIR}/src/example.cpp)
target_include_directories(example PUBLIC ${COMMON_INCLUDES})
target_link_libraries(example PUBLIC ${COMMON_LIBRARIES})
