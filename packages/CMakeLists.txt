
file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/packages/*.cc)
include_directories(${PROJECT_SOURCE_DIR}/packages/include)
add_library(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} PUBLIC glad::glad)
target_link_libraries(${PROJECT_NAME} PUBLIC fmt::fmt)
target_link_libraries(${PROJECT_NAME} PUBLIC SDL2::SDL2)
target_link_libraries(${PROJECT_NAME} PUBLIC $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>)
target_link_libraries(${PROJECT_NAME} PUBLIC $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>)
target_link_libraries(${PROJECT_NAME} PUBLIC $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>)
target_link_libraries(${PROJECT_NAME} PUBLIC glm::glm)
target_link_libraries(${PROJECT_NAME} PUBLIC yaml-cpp::yaml-cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC assimp::assimp)
target_link_libraries(${PROJECT_NAME} PUBLIC LibXml2::LibXml2)
target_link_libraries(${PROJECT_NAME} PUBLIC cjson)
target_link_libraries(${PROJECT_NAME} PUBLIC ${LUA_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PUBLIC imgui::imgui)
