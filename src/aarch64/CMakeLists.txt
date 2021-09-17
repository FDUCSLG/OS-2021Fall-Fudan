file(GLOB aarch64_sources CONFIGURE_DEPENDS "*.c")

add_library(aarch64 STATIC ${aarch64_sources})
