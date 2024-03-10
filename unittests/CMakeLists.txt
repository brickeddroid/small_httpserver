cmake_minimum_required(VERSION 3.5)
include(ExternalProject)
find_package(Git REQUIRED)

set(EXTERNAL_INSTALLATION_DIR ${CMAKE_BINARY_DIR}/thirdparty/utpp/src)

ExternalProject_Add(utpp
    PREFIX ${EXTERNAL_INSTALLATION_DIR}
    GIT_REPOSITORY https://github.com/brickeddroid/utpp
    GIT_TAG master
    TIMEOUT 10
    SOURCE_DIR ${EXTERNAL_INSTALLATION_DIR}/utpp
    UPDATE_COMMAND ${GIT_EXECUTABLE} pull
    CONFIGURE_COMMAND "" # Keine Konfiguration notwendig, wenn das externe Projekt bereits ein CMake-Setup hat
    BUILD_COMMAND ""  # Hier ändern Sie den Build-Befehl entsprechend
    INSTALL_COMMAND "" # Keine Installation notwendig
    LOG_DOWNLOAD ON
)

ExternalProject_Get_Property(utpp BINARY_DIR)

add_executable(unittest test.cpp)

target_include_directories(unittest
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${EXTERNAL_INSTALLATION_DIR}/utpp/include"
)

target_link_libraries(unittest
    PRIVATE
        httpserver
)

if(BUILD_TESTING_HTTP)
    add_test(
        NAME ut_suite_httpserver
        COMMAND $<TARGET_FILE:unittest> HttpMessageTests
    )
else()
    if(BUILD_TESTING_ENUM)
        add_test(
            NAME ut_suite_enumstringify
            COMMAND $<TARGET_FILE:unittest> EnumStringificationTests
        )
    else()
        add_test(
            NAME ut_overall
            COMMAND $<TARGET_FILE:unittest>
        )
    endif()
endif()

# Hinzufügen des 'include'-Verzeichnisses
target_compile_features(unittest PUBLIC cxx_std_20)
