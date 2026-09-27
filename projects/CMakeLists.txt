project(projects C ASM)

# Libraries first, so that every firmware can link against all of them
add_subdirectory(library)
get_property(AVR_LIBRARIES GLOBAL PROPERTY AVR_LIBRARIES)

# Every other folder in here is a firmware: projects/<name>/main.c
# (folders that bring their own CMakeLists.txt manage themselves)
file(GLOB firmware_dirs CONFIGURE_DEPENDS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
     "${CMAKE_CURRENT_SOURCE_DIR}/*")

foreach(firmware_dir IN LISTS firmware_dirs)
  set(firmware_path "${CMAKE_CURRENT_SOURCE_DIR}/${firmware_dir}")
  if(NOT IS_DIRECTORY "${firmware_path}" OR NOT EXISTS "${firmware_path}/main.c")
    continue()
  endif()

  if(EXISTS "${firmware_path}/CMakeLists.txt")
    add_subdirectory("${firmware_dir}")
  else()
    avr_add_firmware(${firmware_dir}
                     SOURCES "${firmware_path}/main.c"
                     LIBRARIES ${AVR_LIBRARIES})
  endif()
endforeach()
