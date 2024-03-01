#add_library(http_server STATIC httpserver.cpp httpmessage.cpp httpconstants.cpp)
#target_compile_features(http_server PUBLIC cxx_std_20)

# my_external_lib/CMakeLists.txt

cmake_minimum_required(VERSION 3.5)
project(HttpServer)

# Hinzufügen von Quelldateien
add_library(httpserver
    STATIC
        src/httpserver.cpp
        src/httpmessage.cpp
        src/httpconstants.cpp
)

# Hinzufügen des 'include'-Verzeichnisses
target_include_directories(httpserver PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
