#add_library(http_server STATIC httpserver.cpp httpmessage.cpp httpconstants.cpp)
#target_compile_features(http_server PUBLIC cxx_std_20)

# my_external_lib/CMakeLists.txt

cmake_minimum_required(VERSION 3.5)
# Hinzufügen von Quelldateien
add_executable(main main.cpp)
target_link_libraries(main PRIVATE httpserver)

# Hinzufügen des 'include'-Verzeichnisses
target_compile_features(main PUBLIC cxx_std_20)


file(COPY ./index.html ./style.css DESTINATION ${CMAKE_BINARY_DIR})
