include_directories(${FF_INCLUDE_DIR})
file(GLOB SOURCES *.cc)
add_library(${FF_PROJECT_NAME} ${SOURCES})