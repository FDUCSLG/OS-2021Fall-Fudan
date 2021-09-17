file(GLOB asm_sources CONFIGURE_DEPENDS "*.S")

add_library(asm STATIC ${asm_sources})
