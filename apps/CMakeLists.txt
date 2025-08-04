add_executable(pvtui_motor motor_display.cpp motor.cpp ../pvtui/pvgroup.cpp ../pvtui/pvtui.cpp)
target_link_libraries(pvtui_motor PRIVATE pvtui)
set_target_properties(pvtui_motor PROPERTIES LINK_FLAGS "-v")

add_executable(pvtui_asyn asyn.cpp ../pvtui/pvgroup.cpp ../pvtui/pvtui.cpp)
target_link_libraries(pvtui_asyn PRIVATE pvtui)
set_target_properties(pvtui_asyn PROPERTIES LINK_FLAGS "-v")

add_executable(pvtui_calcout calcout.cpp ../pvtui/pvtui.cpp ../pvtui/pvgroup.cpp)
target_link_libraries(pvtui_calcout PRIVATE pvtui)
set_target_properties(pvtui_calcout PROPERTIES LINK_FLAGS "-v")

add_executable(pvtui_sr sr.cpp ../pvtui/pvtui.cpp ../pvtui/pvgroup.cpp)
target_link_libraries(pvtui_sr PRIVATE pvtui)
set_target_properties(pvtui_sr PROPERTIES LINK_FLAGS "-v")
