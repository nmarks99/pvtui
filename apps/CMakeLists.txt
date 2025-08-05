target_link_libraries(pvtui_motor PRIVATE pvtui)

add_executable(pvtui_asyn asyn.cpp ../pvtui/pvgroup.cpp ../pvtui/pvtui.cpp)
target_link_libraries(pvtui_asyn PRIVATE pvtui)

add_executable(pvtui_calcout calcout.cpp ../pvtui/pvtui.cpp ../pvtui/pvgroup.cpp)
target_link_libraries(pvtui_calcout PRIVATE pvtui)

add_executable(pvtui_sr sr.cpp ../pvtui/pvtui.cpp ../pvtui/pvgroup.cpp)
target_link_libraries(pvtui_sr PRIVATE pvtui)
