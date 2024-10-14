include_directories(${PROJECT_SOURCE_DIR}/packages/include)
file(GLOB TEST_ITEMS ./src/*.cc)

foreach(TEST_ITEM ${TEST_ITEMS})
    get_filename_component(TEST_ITEM_FILENAME ${TEST_ITEM} NAME)
    string(REGEX REPLACE ".cc" "" TEST_ITEM_FILENAME ${TEST_ITEM_FILENAME})
    add_executable(${FF_MODULE_NAME}.${TEST_ITEM_FILENAME}.test ${TEST_ITEM})
    target_link_libraries(${FF_MODULE_NAME}.${TEST_ITEM_FILENAME}.test firefly)
    target_link_libraries(${FF_MODULE_NAME}.${TEST_ITEM_FILENAME}.test SDL2::SDL2main)
    target_link_libraries(${FF_MODULE_NAME}.${TEST_ITEM_FILENAME}.test unofficial::cunit::cunit)
    add_test(${TEST_ITEM_FILENAME} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${FF_MODULE_NAME}.${TEST_ITEM_FILENAME}.test)
endforeach(TEST_ITEM ${TEST_ITEMS})

