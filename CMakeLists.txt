cmake_minimum_required(VERSION 3.16)

set(MCU "atmega32" CACHE STRING "Target AVR device (atmega32, atmega328p, atmega2560, ...)")
set(F_CPU "8000000" CACHE STRING "CPU clock frequency in Hz (must match the fuses and the clock setup)")
option(AVR_ENABLE_LTO "Build with link time optimization" ON)
set(AVR_PROGRAMMER "atmega32" CACHE STRING "avrdude -p part used by the 'flash' target")
set(AVR_PORT "" CACHE STRING "Serial port used by the 'flash' target, e.g. COM3")

if(NOT CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_LIST_DIR}/cmake/toolchain-avr.cmake")
endif()

project(avr_base C ASM)

if(NOT CMAKE_C_COMPILER_ID STREQUAL "GNU")
  message(FATAL_ERROR "AVR GCC (avr-gcc) is required, got '${CMAKE_C_COMPILER_ID}'")
endif()

get_filename_component(AVR_BIN_DIR "${CMAKE_C_COMPILER}" DIRECTORY)
find_program(AVR_GCC_SIZE   NAMES "avr-size"   HINTS "${AVR_BIN_DIR}")
find_program(AVR_GCC_OBJCOPY NAMES "avr-objcopy" HINTS "${AVR_BIN_DIR}")
find_program(AVR_DUDE      NAMES "avrdude"    HINTS "${AVR_BIN_DIR}")

if(NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/linker/${MCU}.ld")
  set(AVR_LINKER_SCRIPT "" CACHE FILEPATH "Linker script override (empty = avr-lib default)")
else()
  set(AVR_LINKER_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/linker/${MCU}.ld" CACHE FILEPATH "Linker script override (empty = avr-lib default)")
endif()

add_executable(${PROJECT_NAME}
  src/main.c
  src/led.c
  src/sysclock.c
)

target_include_directories(${PROJECT_NAME} PRIVATE
  "${CMAKE_CURRENT_LIST_DIR}/include"
)

target_compile_definitions(${PROJECT_NAME} PRIVATE F_CPU=${F_CPU}UL)

target_compile_options(${PROJECT_NAME} PRIVATE
  -mmcu=${MCU}
  -Os
  -g3
  -std=gnu11
  -Wall
  -Wextra
  -Wundef
  -ffunction-sections
  -fdata-sections
  -funsigned-char
  -funsigned-bitfields
  -fpack-struct
  -fshort-enums
  -fno-common
)

target_link_options(${PROJECT_NAME} PRIVATE
  -mmcu=${MCU}
  -Wl,--gc-sections
  -Wl,--relax
  -Wl,--cref
  -Wl,-Map=${PROJECT_NAME}.map
)

if(AVR_LINKER_SCRIPT)
  target_link_options(${PROJECT_NAME} PRIVATE "-T${AVR_LINKER_SCRIPT}")
endif()

if(AVR_ENABLE_LTO)
  target_compile_options(${PROJECT_NAME} PRIVATE -flto)
  target_link_options(${PROJECT_NAME} PRIVATE -flto)
endif()

if(AVR_GCC_OBJCOPY)
  set(AVR_BIN_OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/bin")
  file(MAKE_DIRECTORY "${AVR_BIN_OUT_DIR}")

  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND "${AVR_GCC_OBJCOPY}" -O ihex -R .eeprom
            $<TARGET_FILE:${PROJECT_NAME}> "${AVR_BIN_OUT_DIR}/${PROJECT_NAME}.hex"
    COMMAND "${AVR_GCC_OBJCOPY}" -O binary -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures
            $<TARGET_FILE:${PROJECT_NAME}> "${AVR_BIN_OUT_DIR}/${PROJECT_NAME}.bin"
    COMMENT "Generating ${PROJECT_NAME}.hex / ${PROJECT_NAME}.bin in ${AVR_BIN_OUT_DIR}"
    VERBATIM
  )
endif()

if(AVR_GCC_SIZE)
  add_custom_target(size
    COMMAND "${AVR_GCC_SIZE}" -B $<TARGET_FILE:${PROJECT_NAME}>
    DEPENDS ${PROJECT_NAME}
    COMMENT "Memory usage"
    VERBATIM
  )
endif()

if(AVR_DUDE)
  set(AVRDUDE_PORT_OPT "")
  if(AVR_PORT)
    set(AVRDUDE_PORT_OPT -P${AVR_PORT})
  endif()
  add_custom_target(flash
    COMMAND "${AVR_DUDE}" -p ${AVR_PROGRAMMER} ${AVRDUDE_PORT_OPT}
            -U flash:w:"${AVR_BIN_OUT_DIR}/${PROJECT_NAME}.hex":i
    DEPENDS ${PROJECT_NAME}
    COMMENT "Flashing ${MCU} on ${AVR_PORT}"
    VERBATIM
  )
  if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/linker/${MCU}-low-fuse.hex"
     AND EXISTS "${CMAKE_CURRENT_LIST_DIR}/linker/${MCU}-high-fuse.hex")
    add_custom_target(fuses
      COMMAND "${AVR_DUDE}" -p ${AVR_PROGRAMMER} ${AVRDUDE_PORT_OPT}
              -U lfuse:w:"${CMAKE_CURRENT_LIST_DIR}/linker/${MCU}-low-fuse.hex":i
              -U hfuse:w:"${CMAKE_CURRENT_LIST_DIR}/linker/${MCU}-high-fuse.hex":i
      COMMENT "Programming fuses of ${MCU}"
      VERBATIM
    )
  endif()
endif()
