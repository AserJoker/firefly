include_directories(${FF_INCLUDE_DIR})
add_executable(${FF_PROJECT_NAME} src/main.cc)
# find_package(SDL2 CONFIG REQUIRED)
# target_link_libraries(${FF_PROJECT_NAME}
#     PRIVATE
#     $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
#     $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
# )  
# find_package(glad CONFIG REQUIRED)
# target_link_libraries(${FF_PROJECT_NAME} PRIVATE glad::glad)
target_link_libraries(${FF_PROJECT_NAME} PRIVATE ${FF_PACKAGES})