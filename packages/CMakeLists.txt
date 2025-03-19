
file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/packages/src/script/*.cc)
# file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/packages/*.cc)
include_directories(${PROJECT_SOURCE_DIR}/packages/include)
add_library(${PROJECT_NAME} ${SOURCES})
# target_link_libraries(${PROJECT_NAME} PUBLIC glad::glad)
# target_link_libraries(${PROJECT_NAME} PUBLIC fmt::fmt)
# target_link_libraries(${PROJECT_NAME} PUBLIC $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>)
# target_link_libraries(${PROJECT_NAME} PUBLIC $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>)
# target_link_libraries(${PROJECT_NAME} PUBLIC glm::glm)
# target_link_libraries(${PROJECT_NAME} PUBLIC Freetype::Freetype)
# target_link_libraries(${PROJECT_NAME} PUBLIC $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
# $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
# )
