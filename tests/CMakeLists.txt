CPMAddPackage(
    NAME doctest
    GITHUB_REPOSITORY onqtam/doctest
    GIT_TAG 2.4.0
)

set(test_target ${PROJECT_NAME}-tests)
set(test_souces
    main.cpp
    engines/jsf_test.cpp
    engines/sfc_test.cpp
    rng_test.cpp
)

add_executable(${test_target} ${test_souces})
set_target_properties(${test_target} PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(${test_target} PRIVATE ${lib_target} doctest)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    target_compile_options(${test_target} PRIVATE
        -Wall
        -Wextra
        -Werror
        -Wconversion
        -Wsign-conversion
        -pedantic-errors
    )
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(${test_target} INTERFACE /W4 /WX)
endif()
