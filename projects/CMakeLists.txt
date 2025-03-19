file(GLOB PROJECTS ${PROJECT_SOURCE_DIR}/projects/*)

foreach(PROJECT ${PROJECTS})
    if(IS_DIRECTORY ${PROJECT})
        get_filename_component(PROJECT ${PROJECT} NAME)
        set(FF_PROJECT_NAME ${PROJECT})
        add_subdirectory(${PROJECT})
    endif()
endforeach(PROJECT ${PROJECTS})

