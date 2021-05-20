add_library(altair8800
  ./88dcdd.h
  ./serial.c
  ./intel8080.h
  ./88dcdd.c
  ./serial.h
  ./roms.h
  ./roms.c
  ./memory.c
  ./memory.h
  ./irq_controller.h
  ./irq_controller.c
  ./intel8080.c
  ./op_codes.h)

target_link_libraries(altair8800 PRIVATE common stm32cube)
target_include_directories(altair8800 PUBLIC ../)
