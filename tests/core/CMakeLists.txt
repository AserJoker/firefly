include_directories(${PROJECT_SOURCE_DIR}/packages/include)
file(GLOB TEST_SUITS ./suits/*.hpp)

add_executable(${FF_MODULE_NAME}.test main.cc)
target_link_libraries(${FF_MODULE_NAME}.test firefly)
target_link_libraries(${FF_MODULE_NAME}.test SDL2::SDL2main)
target_link_libraries(${FF_MODULE_NAME}.test unofficial::cunit::cunit)

foreach(TEST_SUIT ${TEST_SUITS})
    get_filename_component(TEST_SUIT_FILENAME ${TEST_SUIT} NAME)
    string(REGEX REPLACE ".hpp" "" TEST_SUIT_FILENAME ${TEST_SUIT_FILENAME})
    message(Find TEST: ${TEST_SUIT_FILENAME})
    add_test(NAME ${TEST_SUIT_FILENAME}
        COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${FF_MODULE_NAME}.test --suits=${TEST_SUIT_FILENAME})
endforeach(TEST_SUIT ${TEST_SUITS})
