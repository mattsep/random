cmake_minimum_required(VERSION 3.14)

set(namespace "mattsep")
set(package "random")

project(${namespace}-${package}
    LANGUAGES CXX
    VERSION 0.1.0
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(cmake/CPM.cmake)

set(MATTSEP_RANDOM_IS_MAIN_PROJECT OFF)
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(MATTSEP_RANDOM_IS_MAIN_PROJECT ON)
endif()

option(MATTSEP_RANDOM_BUILD_TESTS "Build the mattsep::random test executable" ${MATTSEP_RANDOM_IS_MAIN_PROJECT})

# -----------------------------------------------------------------------------
# define target

function(create_target target)
    add_library(${target} INTERFACE)
    target_include_directories(${target} ${ARGN} INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )
    target_compile_features(${target} INTERFACE cxx_std_17)
endfunction()

if(MATTSEP_RANDOM_IS_MAIN_PROJECT AND NOT TARGET ${package})
    set(lib_target ${package})
else()
    set(lib_target ${namespace}::${package})
endif()

add_library(${lib_target} INTERFACE)
target_include_directories(${lib_target} ${ARGN} INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_compile_features(${lib_target} INTERFACE cxx_std_17)

# -----------------------------------------------------------------------------
# testing

if(MATTSEP_RANDOM_BUILD_TESTS)
    target_compile_definitions(${lib_target} INTERFACE MATTSEP_RANDOM_BUILD_TESTS)
    add_subdirectory(tests)
endif()

# -----------------------------------------------------------------------------
# installation

if(MATTSEP_RANDOM_IS_MAIN_PROJECT)
    set(config_install_dir ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

    write_basic_package_version_file(
        ${PROJECT_NAME}-config-version.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
    )

    configure_package_config_file(
        ${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}-config.cmake.in
        ${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
        INSTALL_DESTINATION ${config_install_dir}
    )

    install(
        TARGETS ${package}
        EXPORT ${PROJECT_NAME}-targets
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

    install(
        EXPORT ${PROJECT_NAME}-targets
        FILE ${PROJECT_NAME}-targets.cmake
        NAMESPACE ${namespace}::
        DESTINATION ${config_install_dir}
    )

    install(
        FILES
            ${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
            ${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
        DESTINATION ${config_install_dir}
    )

    install(
        DIRECTORY ${PROJECT_SOURCE_DIR}/include/${namespace}/${package}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${namespace}
    )
endif()
