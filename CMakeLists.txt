cmake_minimum_required(VERSION 3.7)

if( CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR )
    project(HttpServer)
    message("EXTERNAL PROJECT TRIGGERED")
    include(CTest)

    if(HTTP_SERVER_PROJECT_BUILD_TESTING)
    add_subdirectory(test)
    # or alternatively, `include(CTest)`, if you want to use CDash
    # https://cmake.org/cmake/help/book/mastering-cmake/chapter/CDash.html
    endif()

    if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
        add_subdirectory(unittests)
    endif()
    enable_testing()

endif()

# Hinzufügen von Quelldateien
add_library(httpserver
    STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/httpmessage.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/httpserver.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/include/enumstringifier.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/httpconstants.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/httpmessage.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/httpserver.hpp
)

# Hinzufügen des 'include'-Verzeichnisses
target_include_directories(httpserver
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_compile_features(httpserver PUBLIC cxx_std_20)
