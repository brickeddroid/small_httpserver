add_library(http_server STATIC httpserver.cpp httpmessage.cpp httpconstants.cpp)
target_compile_features(http_server PUBLIC cxx_std_20)
