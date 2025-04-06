include(FetchContent)

set(CATCH_USE_VALGRIND OFF CACHE BOOL "Perform SelfTests with Valgrind")
set(CATCH_BUILD_TESTING OFF CACHE BOOL "Build SelfTest project")
set(CATCH_BUILD_EXAMPLES OFF CACHE BOOL "Build documentation examples")
set(CATCH_BUILD_EXTRA_TESTS OFF CACHE BOOL "Build extra tests")
set(CATCH_ENABLE_COVERAGE OFF CACHE BOOL "Generate coverage for codecov.io")
set(CATCH_ENABLE_WERROR OFF CACHE BOOL "Enable all warnings as errors")
set(CATCH_INSTALL_DOCS OFF CACHE BOOL "Install documentation alongside library")
set(CATCH_INSTALL_HELPERS OFF CACHE BOOL "Install contrib alongside library")

FetchContent_Declare(
    Catch2 GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.8.0 # or a later release
)

FetchContent_MakeAvailable(Catch2)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} PARENT_SCOPE)
