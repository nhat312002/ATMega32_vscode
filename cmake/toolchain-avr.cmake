# CMake cross-compile toolchain for AVR (avr-gcc from avr8-gnu-toolchain-win32)
#
# Usage:
#   cmake -B build -G Ninja -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-avr.cmake
#
# If avr-gcc is not in PATH, point CMake at it directly:
#   cmake -B build -G Ninja -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-avr.cmake -DAVR_GCC=C:/path/to/avr-gcc.exe

set(CMAKE_SYSTEM_NAME      Generic)
set(CMAKE_SYSTEM_PROCESSOR avr)

if(NOT CMAKE_C_COMPILER)
  if(NOT AVR_GCC)
    find_program(AVR_GCC NAMES avr-gcc)
  endif()
  if(NOT AVR_GCC)
    message(FATAL_ERROR
      "avr-gcc not found. Add it to PATH or pass -DAVR_GCC=<full path to avr-gcc.exe>")
  endif()

  get_filename_component(AVR_TOOLCHAIN_BIN_DIR "${AVR_GCC}" DIRECTORY)
  find_program(AVR_AR       NAMES avr-ar       avr-gcc-ar       HINTS "${AVR_TOOLCHAIN_BIN_DIR}")
  find_program(AVR_RANLIB   NAMES avr-ranlib   avr-gcc-ranlib   HINTS "${AVR_TOOLCHAIN_BIN_DIR}")
  find_program(AVR_OBJCOPY  NAMES avr-objcopy  avr-gcc-objcopy  HINTS "${AVR_TOOLCHAIN_BIN_DIR}")
  find_program(AVR_OBJDUMP  NAMES avr-objdump  avr-gcc-objdump  HINTS "${AVR_TOOLCHAIN_BIN_DIR}")
  find_program(AVR_SIZE     NAMES avr-size                    HINTS "${AVR_TOOLCHAIN_BIN_DIR}")

  set(CMAKE_C_COMPILER   "${AVR_GCC}"  CACHE FILEPATH "C compiler")
  set(CMAKE_ASM_COMPILER "${AVR_GCC}"  CACHE FILEPATH "ASM compiler")
  if(AVR_AR)
    set(CMAKE_AR "${AVR_AR}" CACHE FILEPATH "Archiver")
  endif()
  if(AVR_RANLIB)
    set(CMAKE_RANLIB "${AVR_RANLIB}" CACHE FILEPATH "Ranlib")
  endif()
  if(AVR_OBJCOPY)
    set(CMAKE_OBJCOPY "${AVR_OBJCOPY}" CACHE FILEPATH "objcopy")
  endif()
  if(AVR_OBJDUMP)
    set(CMAKE_OBJDUMP "${AVR_OBJDUMP}" CACHE FILEPATH "objdump")
  endif()
  if(AVR_SIZE)
    set(CMAKE_SIZE "${AVR_SIZE}" CACHE FILEPATH "size")
  endif()
endif()

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# The device specific startup code and linker script make the executable link
# test unreliable, so the compiler ABI check is done with a static library.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
