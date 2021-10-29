file(GLOB user_sources CONFIGURE_DEPENDS "*.S")

add_library(user STATIC ${user_sources})
