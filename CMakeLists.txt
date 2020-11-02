cmake_minimum_required(VERSION 3.14)

set(package msep)
set(component random)

project(${package}-${component}
    LANGUAGES CXX
    VERSION 0.1.0
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(cmake/CPM.cmake)

set(MSEP_RANDOM_IS_MAIN_PROJECT OFF)
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(MSEP_RANDOM_IS_MAIN_PROJECT ON)
endif()

option(MSEP_RANDOM_BUILD_TESTS "Build the msep::random test executable" ${MSEP_RANDOM_IS_MAIN_PROJECT})

# -----------------------------------------------------------------------------
# define target

if(MSEP_RANDOM_IS_MAIN_PROJECT)
    if(NOT TARGET ${component})
        set(target_name ${component})
    else()
        message(FATAL_ERROR "msep::random is the main project, but target 'random' already exists!")
    endif()
else()
    set(target_name ${package}-${component})
endif()

function(create_target target)
    add_library(${target} INTERFACE)
    target_include_directories(${target} ${ARGN} INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )
    target_compile_features(${target} INTERFACE cxx_std_17)
endfunction()

create_target(${target_name} SYSTEM)
add_library(${package}::${component} ALIAS ${target_name})

# -----------------------------------------------------------------------------
# testing

if(MSEP_RANDOM_BUILD_TESTS)
    add_subdirectory(tests)
endif()

# -----------------------------------------------------------------------------
# installation

if(MSEP_RANDOM_IS_MAIN_PROJECT)
    set(config_install_dir ${CMAKE_INSTALL_DATADIR}/cmake/${package})

    # NOTE: Currently, cmake doesn't support per-component version numbers.
    # We'll make a config-version file for the component anyway, and hope that
    # future cmake versions will add support.
    write_basic_package_version_file(
        "${package}-${component}-config-version.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
    )

    configure_package_config_file(
        ${PROJECT_SOURCE_DIR}/cmake/${package}-config.cmake.in
        ${PROJECT_BINARY_DIR}/${package}-config.cmake
        INSTALL_DESTINATION ${config_install_dir}
    )

    configure_package_config_file(
        ${PROJECT_SOURCE_DIR}/cmake/${package}-${component}-config.cmake.in
        ${PROJECT_BINARY_DIR}/${package}-${component}-config.cmake
        INSTALL_DESTINATION ${config_install_dir}
    )

    install(
        TARGETS ${component}
        EXPORT ${component}-targets
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

    install(
        EXPORT ${component}-targets
        FILE ${package}-${component}-targets.cmake
        NAMESPACE ${package}::
        DESTINATION ${config_install_dir}
        COMPONENT ${component}
    )

    install(
        FILES
            ${PROJECT_BINARY_DIR}/${package}-config.cmake
            ${PROJECT_BINARY_DIR}/${package}-${component}-config.cmake
            ${PROJECT_BINARY_DIR}/${package}-${component}-config-version.cmake
        DESTINATION ${config_install_dir}
        COMPONENT ${component}
    )

    install(
        DIRECTORY ${PROJECT_SOURCE_DIR}/include/${package}/${component}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${package}
    )
endif()
