include_directories(${PROJECT_SOURCE_DIR}/packages/include)
file(GLOB TEST_SUITS ./suits/*.hpp)

set(SUITS "")

foreach(TEST_SUIT ${TEST_SUITS})
    get_filename_component(TEST_SUIT_FILENAME ${TEST_SUIT} NAME)
    string(REGEX REPLACE ".hpp" "" TEST_SUIT_FILENAME ${TEST_SUIT_FILENAME})
    list(APPEND SUITS ${TEST_SUIT_FILENAME})
    add_test(NAME ${TEST_SUIT_FILENAME}
        COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${FF_MODULE_NAME}.test --suits=${TEST_SUIT_FILENAME})
endforeach(TEST_SUIT ${TEST_SUITS})

set(TEST_INCLUDE_LIST "")
set(TEST_REGISTER_LIST "")

foreach(SUIT ${SUITS})
    message("FIND SUIT: ${SUIT}")
    string(APPEND TEST_INCLUDE_LIST "#include \"suits/${SUIT}.hpp\"\n")
    string(JOIN "," TEST_REGISTER_LIST ${SUITS})
endforeach(SUIT ${SUITS})

configure_file(${PROJECT_SOURCE_DIR}/tests/entry.cc.in ${CMAKE_CURRENT_SOURCE_DIR}/entry.cc)

add_executable(${FF_MODULE_NAME}.test entry.cc)
target_link_libraries(${FF_MODULE_NAME}.test firefly)
target_link_libraries(${FF_MODULE_NAME}.test SDL2::SDL2main)
target_link_libraries(${FF_MODULE_NAME}.test unofficial::cunit::cunit)
