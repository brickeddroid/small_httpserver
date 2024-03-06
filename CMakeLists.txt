#add_library(http_server STATIC httpserver.cpp httpmessage.cpp httpconstants.cpp)
#target_compile_features(http_server PUBLIC cxx_std_20)

# my_external_lib/CMakeLists.txt

cmake_minimum_required(VERSION 3.5)
project(HttpServer)

set(HTTP_SERVER_PROJECT_BUILD_TESTING true)

# Hinzufügen von Quelldateien
add_library(httpserver
    STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/httpserver.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/httpmessage.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/httpconstants.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/include/httpserver.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/httpmessage.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/httpconstants.hpp
)



if(HTTP_SERVER_PROJECT_BUILD_TESTING)
  add_subdirectory(test)
  # or alternatively, `include(CTest)`, if you want to use CDash
  # https://cmake.org/cmake/help/book/mastering-cmake/chapter/CDash.html
endif()

# Hinzufügen des 'include'-Verzeichnisses
target_include_directories(httpserver
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_compile_features(httpserver PUBLIC cxx_std_20)
