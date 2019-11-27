cmake_minimum_required(VERSION 3.13)
project(wolf_clone)
set(CMAKE_CXX_STANDARD 11)

include(ExternalProject)
ExternalProject_Add(_wolf
        GIT_REPOSITORY ssh://git@stash.ysoft.local/mp/wolf.git
        #        GIT_TAG 2019-03-01
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
        PREFIX ${WOLF_CLONE_DIR}
        )