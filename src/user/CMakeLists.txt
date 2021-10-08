file(GLOB asm_sources CONFIGURE_DEPENDS "*.S")

add_library(user STATIC ${asm_sources})
