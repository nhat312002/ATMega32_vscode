cmake_minimum_required(VERSION 3.16)

set(MCU "atmega32" CACHE STRING "Target AVR device (atmega32, atmega328p, atmega2560, ...)")
set(F_CPU "8000000" CACHE STRING "CPU clock frequency in Hz (must match the fuses and the clock setup)")
option(AVR_ENABLE_LTO "Build with link time optimization (requires the LTO plugin in avr-ar)" OFF)
set(AVR_PROGRAMMER "atmega32" CACHE STRING "avrdude -p part used by the 'flash' targets")
set(AVR_PORT "" CACHE STRING "Serial port used by the 'flash' targets, e.g. COM3")

if(NOT CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_LIST_DIR}/cmake/toolchain-avr.cmake")
endif()

project(avr_base C ASM)

if(NOT CMAKE_C_COMPILER_ID STREQUAL "GNU")
  message(FATAL_ERROR "AVR GCC (avr-gcc) is required, got '${CMAKE_C_COMPILER_ID}'")
endif()

get_filename_component(AVR_BIN_DIR "${CMAKE_C_COMPILER}" DIRECTORY)
find_program(AVR_GCC_SIZE    NAMES "avr-size"    HINTS "${AVR_BIN_DIR}")
find_program(AVR_GCC_OBJCOPY NAMES "avr-objcopy" HINTS "${AVR_BIN_DIR}")
find_program(AVR_DUDE        NAMES "avrdude"     HINTS "${AVR_BIN_DIR}")

if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/linker/${MCU}.ld")
  set(AVR_LINKER_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/linker/${MCU}.ld" CACHE FILEPATH "Linker script (empty = avr-lib default)")
else()
  set(AVR_LINKER_SCRIPT "" CACHE FILEPATH "Linker script (empty = avr-lib default)")
endif()

# Settings shared with every target defined in projects/
set(AVR_BIN_OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/bin")
file(MAKE_DIRECTORY "${AVR_BIN_OUT_DIR}")

set(AVR_COMMON_DEFINITIONS F_CPU=${F_CPU}UL)
set(AVR_COMPILE_OPTIONS
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
set(AVR_LINK_OPTIONS
  -mmcu=${MCU}
  -Wl,--gc-sections
  -Wl,--relax
  -Wl,--cref
)
if(AVR_ENABLE_LTO)
  list(APPEND AVR_COMPILE_OPTIONS -flto)
  list(APPEND AVR_LINK_OPTIONS -flto)
endif()
if(AVR_LINKER_SCRIPT)
  list(APPEND AVR_LINK_OPTIONS "-T${AVR_LINKER_SCRIPT}")
endif()

# Creates a firmware: avr_add_firmware(<name> <source> [LIBRARY...])
# plus the matching size-<name> / flash-<name> targets
function(avr_add_firmware target_name)
  set(options "")
  set(one_value_args "")
  set(multi_value_args SOURCES LIBRARIES)
  cmake_parse_arguments(FIRMWARE "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

  set(sources ${FIRMWARE_SOURCES})
  if(NOT sources)
    set(sources "${CMAKE_CURRENT_SOURCE_DIR}/main.c")
  endif()

  message(STATUS "Firmware project: ${target_name}")
  add_executable(${target_name} ${sources})

  target_compile_definitions(${target_name} PRIVATE ${AVR_COMMON_DEFINITIONS})
  target_compile_options(${target_name} PRIVATE ${AVR_COMPILE_OPTIONS})
  foreach(header_dir IN ITEMS "inc" "include")
    if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${header_dir}")
      target_include_directories(${target_name} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/${header_dir}")
    endif()
  endforeach()

  target_link_libraries(${target_name} PRIVATE ${FIRMWARE_LIBRARIES})
  target_link_options(${target_name} PRIVATE ${AVR_LINK_OPTIONS} "-Wl,-Map=${target_name}.map")

  if(AVR_GCC_OBJCOPY)
    add_custom_command(TARGET ${target_name} POST_BUILD
      COMMAND "${AVR_GCC_OBJCOPY}" -O ihex -R .eeprom
              $<TARGET_FILE:${target_name}> "${AVR_BIN_OUT_DIR}/${target_name}.hex"
      COMMAND "${AVR_GCC_OBJCOPY}" -O binary -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures
              $<TARGET_FILE:${target_name}> "${AVR_BIN_OUT_DIR}/${target_name}.bin"
      COMMENT "Generating ${target_name}.hex / ${target_name}.bin in ${AVR_BIN_OUT_DIR}"
      VERBATIM
    )
  endif()

  if(AVR_GCC_SIZE)
    add_custom_target(size-${target_name}
      COMMAND "${AVR_GCC_SIZE}" -B $<TARGET_FILE:${target_name}>
      DEPENDS ${target_name}
      COMMENT "Memory usage of ${target_name}"
      VERBATIM
    )
    set_property(GLOBAL APPEND PROPERTY AVR_SIZE_TARGETS size-${target_name})
  endif()

  if(AVR_DUDE)
    set(AVRDUDE_PORT_OPT "")
    if(AVR_PORT)
      set(AVRDUDE_PORT_OPT -P${AVR_PORT})
    endif()
    add_custom_target(flash-${target_name}
      COMMAND "${AVR_DUDE}" -p ${AVR_PROGRAMMER} ${AVRDUDE_PORT_OPT}
              -U flash:w:"${AVR_BIN_OUT_DIR}/${target_name}.hex":i
      DEPENDS ${target_name}
      COMMENT "Flashing ${target_name} to ${MCU}"
      VERBATIM
    )
    set_property(GLOBAL APPEND PROPERTY AVR_FLASH_TARGETS flash-${target_name})
  endif()

  set_property(GLOBAL APPEND PROPERTY AVR_FIRMWARE_TARGETS ${target_name})
endfunction()

add_subdirectory(projects)

get_property(AVR_FIRMWARE_TARGETS GLOBAL PROPERTY AVR_FIRMWARE_TARGETS)
if(NOT AVR_FIRMWARE_TARGETS)
  message(WARNING "No firmware found: create projects/<name>/main.c")
endif()

get_property(AVR_SIZE_TARGETS GLOBAL PROPERTY AVR_SIZE_TARGETS)
if(AVR_SIZE_TARGETS)
  add_custom_target(size DEPENDS ${AVR_SIZE_TARGETS})
endif()

get_property(AVR_FLASH_TARGETS GLOBAL PROPERTY AVR_FLASH_TARGETS)
if(AVR_FLASH_TARGETS)
  add_custom_target(flash DEPENDS ${AVR_FLASH_TARGETS})
endif()

if(AVR_DUDE
   AND EXISTS "${CMAKE_CURRENT_LIST_DIR}/linker/${MCU}-low-fuse.hex"
   AND EXISTS "${CMAKE_CURRENT_LIST_DIR}/linker/${MCU}-high-fuse.hex")
  add_custom_target(fuses
    COMMAND "${AVR_DUDE}" -p ${AVR_PROGRAMMER} ${AVRDUDE_PORT_OPT}
            -U lfuse:w:"${CMAKE_CURRENT_LIST_DIR}/linker/${MCU}-low-fuse.hex":i
            -U hfuse:w:"${CMAKE_CURRENT_LIST_DIR}/linker/${MCU}-high-fuse.hex":i
    COMMENT "Programming fuses of ${MCU}"
    VERBATIM
  )
endif()
