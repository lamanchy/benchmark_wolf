cmake_minimum_required(VERSION 3.13)
project(benchmark_wolf)
set(CMAKE_CXX_STANDARD 11)

# check for current platform
if (WIN32)
    set(WOLF_SYSTEM windows)
elseif (UNIX)
    set(WOLF_SYSTEM linux)
else ()
    message(FATAL_ERROR "Unknown system")
endif ()

# add wolf
include(cmake/add_wolf.cmake)

# make src/*.cpp files targets
file(GLOB executables "${CMAKE_SOURCE_DIR}/src/*.cpp")
foreach(executable ${executables})
    get_filename_component(target ${executable} NAME_WE)

    # local executable + headers
    add_executable(${target} ${CMAKE_SOURCE_DIR}/src/${target}.cpp)
    target_include_directories(${target} PRIVATE ${CMAKE_SOURCE_DIR}/include)

    # wolf headers + lib
    target_include_directories(${target} PRIVATE ${WOLF_PATH}/include)
    target_link_libraries(${target} wolf)

    # wolfs libs headers + libs
    target_include_directories(${target} SYSTEM PRIVATE ${WOLF_LIB_HEADERS})
    target_link_libraries(${target} ${WOLF_LIBS})

    set(WOLF_TARGET_DIR ${target}-${WOLF_SYSTEM})
    if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
        set(WOLF_TARGET_DIR ${WOLF_TARGET_DIR}-debug)
    endif ()

    install(TARGETS ${target} DESTINATION ${WOLF_TARGET_DIR}/)

    if (${target} STREQUAL parser)
        install(FILES configs/parsers DESTINATION ${WOLF_TARGET_DIR}/)
        install(FILES configs/elapsed DESTINATION ${WOLF_TARGET_DIR}/)
    endif ()
    if (${target} STREQUAL little_stack_parser)
        install(FILES configs/parsers DESTINATION ${WOLF_TARGET_DIR}/)
        install(FILES configs/elapsed DESTINATION ${WOLF_TARGET_DIR}/)
    endif ()
endforeach ()