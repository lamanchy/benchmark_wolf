
if (NOT WOLF_PATH)
    include(cmake/get_wolf/CMakeLists.txt)

    set(WOLF_PATH ${CMAKE_BINARY_DIR}/_wolf-prefix/src/_wolf)
    if (NOT EXISTS ${WOLF_PATH}/CMakeLists.txt)
        set(WOLF_CLONE_DIR ${CMAKE_BINARY_DIR}/_wolf-prefix)
        file(MAKE_DIRECTORY ${WOLF_CLONE_DIR})
        execute_process(
                COMMAND ${CMAKE_COMMAND}
                -G ${CMAKE_GENERATOR}
                -DWOLF_CLONE_DIR=${WOLF_CLONE_DIR}
                ${CMAKE_CURRENT_SOURCE_DIR}/cmake/get_wolf

                WORKING_DIRECTORY ${WOLF_CLONE_DIR}
                RESULT_VARIABLE RESULT
        )
        if (NOT ${RESULT} EQUAL "0")
            message(FATAL_ERROR "Wolf clone configuration failed")
        endif ()
        execute_process(
                COMMAND ${CMAKE_COMMAND} --build . --config ${CMAKE_BUILD_TYPE}

                WORKING_DIRECTORY ${WOLF_CLONE_DIR}
                RESULT_VARIABLE RESULT
        )
        if (NOT ${RESULT} EQUAL "0")
            message(FATAL_ERROR "Wolf clone failed")
        endif ()
    endif ()
endif ()

# this add correct compile flags, and sets few variables to link/include
add_subdirectory(${WOLF_PATH}/cmake/build_wolf wolf_lib EXCLUDE_FROM_ALL)