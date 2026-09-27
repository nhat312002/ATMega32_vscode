project(library C ASM)

# Add the sources of this library here
add_library(library STATIC
  src/i2c.c
  src/spi.c
)

# Add the include paths of this library here
target_include_directories(library PUBLIC
  inc
)

target_compile_definitions(library PUBLIC ${AVR_COMMON_DEFINITIONS})
target_compile_options(library PRIVATE ${AVR_COMPILE_OPTIONS})

# Let the firmwares in ../ pick this library up automatically
set_property(GLOBAL APPEND PROPERTY AVR_LIBRARIES library)
