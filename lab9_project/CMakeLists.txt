# add_executable(lab9_frogtest.elf main_frogtest.c frog.c graphics.c)
# target_link_libraries(lab9_frogtest.elf ${330_LIBS} touchscreen interrupts intervalTimer buttons_switches)
# set_target_properties(lab9_frogtest.elf PROPERTIES LINKER_LANGUAGE CXX)

add_executable(lab9.elf main.c gameControl.c frog.c graphics.c carsRow.c)
target_link_libraries(lab9.elf ${330_LIBS} touchscreen interrupts intervalTimer buttons_switches)
set_target_properties(lab9.elf PROPERTIES LINKER_LANGUAGE CXX)

#add_executable(lab9_objecttest.elf main_objecttest.c object.c)
#target_link_libraries(lab9_objecttest.elf ${330_LIBS} interrupts intervalTimer buttons_switches)
#set_target_properties(lab9_objecttest.elf PROPERTIES LINKER_LANGUAGE CXX)
