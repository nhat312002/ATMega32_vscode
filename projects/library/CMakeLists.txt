project(library C ASM)

# Shared code: every .c in src/ is compiled once and linked into the firmwares
file(GLOB library_sources CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.S")

if(NOT library_sources)
  message(FATAL_ERROR "No source file found in ${CMAKE_CURRENT_SOURCE_DIR}/src")
endif()

add_library(library STATIC ${library_sources})

foreach(header_dir IN ITEMS "inc" "include")
  if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${header_dir}")
    target_include_directories(library PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/${header_dir}")
  endif()
endforeach()

target_compile_definitions(library PUBLIC ${AVR_COMMON_DEFINITIONS})
target_compile_options(library PRIVATE ${AVR_COMPILE_OPTIONS})

# Let the firmwares in ../ pick this library up automatically
set_property(GLOBAL APPEND PROPERTY AVR_LIBRARIES library)
