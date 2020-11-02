CPMAddPackage(
    NAME doctest
    GITHUB_REPOSITORY onqtam/doctest
    GIT_TAG 2.4.0
)

set(test_target ${PROJECT_NAME}-tests)

create_target(${test_target}-lib)
target_compile_definitions(${test_target}-lib INTERFACE MSEP_RANDOM_BUILD_TESTS)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    target_compile_options(${test_target}-lib INTERFACE
        -Wall
        -Wextra
        -Werror
        -Wconversion
        -Wsign-conversion
        -pedantic-errors
    )
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(${test_target}-lib INTERFACE /W4 /WX)
endif()

add_executable(${test_target} main.cpp)
target_link_libraries(${test_target} PRIVATE ${test_target}-lib doctest)
