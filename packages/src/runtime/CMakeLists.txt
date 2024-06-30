include_directories(${FF_INCLUDE_DIR})
file(GLOB SOURCES *.cc)
add_library(${FF_PROJECT_NAME} ${SOURCES})
target_link_libraries(${FF_PROJECT_NAME} ${FF_ROOT_PROJECT_NAME}.core)