#include <iostream>
#include <vector>
#include <future>

#include "httpserver.hpp"

using namespace YatePlugin;

int main(int argc, const char* argv[]) {

    //std::cout << YatePlugin::Esper2ToString(YatePlugin::Esper2::Unu) << std::endl;

    std::vector<std::future<void>> futures;
    bool accepting = false;
    SOCKET server_sock;
    HttpServer server("0.0.0.0", 8082);

    auto provide_file = [](const HttpRequest& http_request) -> HttpResponse {
        std::cout << "Preparing file for serving..." << http_request.path() << std::endl;
        HttpResponse http_response;
        http_response.set_status_code(HttpStatusCode::Ok);
        const std::string& accept_content = http_request.header("Accept");
        std::string mime_type = accept_content.substr(0, accept_content.find(","));
        std::string response_content = "";
        std::string path = http_request.path();
        if(path == "/"){
            path = "./index.html";
        } else {
            path.insert(0, ".");
        }

        if (std::ifstream ifs { path }) {
            std::cout << "File opened " << http_request.path() << std::endl;
            std::string data((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
            // use data here
            std::cout << data << '\n';
            http_response.set_content(data);
        } else {
            http_response.set_status_code(HttpStatusCode::NotFound);
            std::cout << "Cannot open file\n";
        }
        http_response.add_header("Content-Length", std::to_string(response_content.length()));
        http_response.add_header("Content-Type", mime_type);
        return http_response;
    };

    auto provide_open_file = [](const HttpRequest& http_request) -> HttpResponse {
        std::cout << "Preparing file for serving..." << http_request.path() << std::endl;
        HttpResponse http_response;
        http_response.set_status_code(HttpStatusCode::Ok);
        const std::string& accept_content = http_request.header("Accept");
        std::string mime_type = accept_content.substr(0, accept_content.find(","));
        std::string response_content = "";
        std::string path = http_request.query_parameter("file");
        if(path == "/"){
            path = "./index.html";
        } else {
            path.insert(0, ".");
        }

        if (std::ifstream ifs { path }) {
            std::cout << "File opened " << http_request.path() << std::endl;
            std::string data((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
            // use data here
            std::cout << data << '\n';
            http_response.set_content(data);
        } else {
            http_response.set_status_code(HttpStatusCode::NotFound);
            std::cout << "Cannot open file\n";
        }
        http_response.add_header("Content-Length", std::to_string(response_content.length()));
        http_response.add_header("Content-Type", mime_type);
        return http_response;
    };

    server.register_uri_handler("/", HttpMethod::GET, provide_file);
    server.register_uri_handler("/style.css", HttpMethod::GET, provide_file);
    server.register_uri_handler("/open", HttpMethod::GET, provide_open_file);


    server.initialize_socket();
    server.open_socket(server_sock, accepting);
    server.bind_socket(server_sock);
    server.listen_socket(futures, server_sock, accepting);


    std::cout << "bye!" << std::endl;
    exit(EXIT_SUCCESS);

};
