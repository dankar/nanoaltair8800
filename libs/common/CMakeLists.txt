add_library(common
  ./config/config.h
  ./config/config.c
  ./sys/atomic_lock.h
  ./sys/switches.c
  ./sys/spi.c
  ./sys/log.h
  ./sys/switches.h
  ./sys/usb.c
  ./sys/profiler.c
  ./sys/profiler.h
  ./sys/log.c
  ./sys/arm/filesystem.c
  ./sys/hw_timer.c
  ./sys/buffer.c
  ./sys/timer.h
  ./sys/spi.h
  ./sys/buffer.h
  ./sys/hw_timer.h
  ./sys/filesystem.h
  ./sys/usb.h
  ./sys/syscalls.c)

target_link_libraries(common PRIVATE stm32cube altair8800)

target_include_directories(common PUBLIC .)
