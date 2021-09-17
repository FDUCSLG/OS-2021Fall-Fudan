file(GLOB core_sources CONFIGURE_DEPENDS "*.c")

add_library(core STATIC ${core_sources})
