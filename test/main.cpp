#include <iostream>
#include <vector>
#include <future>

#include "httpserver.hpp"

int main(int argc, const char* argv[]) {
    HttpServer server("0.0.0.0", 8083);

    auto provide_file = [](const HttpRequest& http_request, HttpResponse& http_response) {
        std::cout << "Preparing file for serving..." << http_request.path() << std::endl;
        http_response.set_status_code(HttpStatusCode::Ok);
        const std::string& accept_content = http_request.header("Accept");
        std::string mime_type = accept_content.substr(0, accept_content.find(","));
        std::string path = http_request.path();
        if(path == "/"){
            path = "./test/index.html";
        } else {
            path.insert(0, "./test");
        }

        if (std::ifstream ifs { path }) {
            std::cout << "File opened " << http_request.path() << std::endl;
            std::string data((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
            // use data here
            //std::cout << data << '\n';
            http_response.set_content(data);
        } else {
            http_response.set_status_code(HttpStatusCode::NotFound);
            std::cout << "Cannot open file " << path << std::endl;
        }
        http_response.add_header("Content-Length", std::to_string(http_response.content().length()));
        http_response.add_header("Content-Type", mime_type);
    };

    auto provide_open_file = [](const HttpRequest& http_request, HttpResponse& http_response) {
        std::cout << "Preparing file for serving..." << http_request.query_parameter("file") << std::endl;
        http_response.set_status_code(HttpStatusCode::Ok);
        const std::string& accept_content = http_request.header("Accept");
        std::string mime_type = accept_content.substr(0, accept_content.find(","));
        std::string response_content = "";
        std::string path = http_request.query_parameter("file");
        if(path == "/"){
            path = "./test/index.html";
        } else {
            path.insert(0, "./test");
        }

        if (std::ifstream ifs { path }) {
            std::cout << "File opened " << path << std::endl;
            std::string data((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
            // use data here
            //std::cout << data << '\n';
            http_response.set_content(data);
        } else {
            http_response.set_status_code(HttpStatusCode::NotFound);
            std::cout << "Cannot open file\n";
        }
        http_response.add_header("Content-Length", std::to_string(http_response.content().length()));
        http_response.add_header("Content-Type", mime_type);
    };

    auto register_event = [](const HttpRequest& http_request, HttpResponse& http_response) {
            http_response.set_status_code(HttpStatusCode::Ok);
            http_response.set_version(HttpVersion::Http_11);
            http_response.add_header("Cache-Control", "no-store");
            http_response.add_header("Content-Type", "text/event-stream");
            const std::string response = "";//"event: ping\ndata: 'Hello world'\n\n";
            http_response.add_header("Content-Length", std::to_string(response.length()));
            http_response.set_content(response);
            http_response.set_body_only(true);
    };
    server.register_uri_handler("/", HttpMethod::GET, provide_file);
    server.register_uri_handler("/style.css", HttpMethod::GET, provide_file);
    server.register_uri_handler("/open", HttpMethod::GET, provide_open_file);

    server.register_uri_handler("/event", HttpMethod::GET, register_event);

    server.start();

    std::cout << "bye!" << std::endl;
    exit(EXIT_SUCCESS);

};
