#include <iostream>
//#include <vector>
//#include <future>
#include <signal.h>

#include "../include/httpserver.hpp"

Http::Server server("0.0.0.0", 8083);

void sigpipe_handler(int signum){
    std::cout << "Interrupt signal (" << signum << ") received.\n";

    server.stop();

    exit(signum);
}

void sigint_handler( int signum ) {
   std::cout << "Interrupt signal (" << signum << ") received on thread " << std::this_thread::get_id() << ".\n";

   // cleanup and close up stuff here
   // terminate program
   server.stop();

   std::cout << "Server stopped" << std::endl;

   exit(signum);
}

int main(int argc, const char* argv[]) {

    signal(SIGPIPE, sigpipe_handler);
    signal(SIGINT, sigint_handler);

    std::cout << "Welcome" << std::endl;

    Http::EventSource event_source;
    server.register_event("/event", &event_source);
    std::cout << "Events registered" << std::endl;

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
            http_response.add_header("Cache-Control", "no-cache");
            http_response.add_header("Content-Type", "text/event-stream");
            const std::string response = "";//"event: ping\ndata: 'Hello world'\n\n";
            //http_response.add_header("Content-Length", std::to_string(response.length()));
            http_response.set_content(response);
            //http_response.set_body_only(true);
    };
    server.register_uri_handler("/", HttpMethod::GET, provide_file);
    server.register_uri_handler("/style.css", HttpMethod::GET, provide_file);
    server.register_uri_handler("/open", HttpMethod::GET, provide_open_file);

    //server.register_uri_handler("/event", HttpMethod::GET, register_event);
    //HttpEventSource event_source("/event");
    //server.register_handler(&event_source);
    //server.start();

    try {
        server.start();
        std::cout << "Server started" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        static int counter = 0;
        while(server.status() == Http::Server::Status::RUNNING){
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::stringstream ss;
            ss << "data: " << counter++ << "\n\n";
            event_source.send_event(ss.str());
        }
    } catch(std::runtime_error e){
        Http::error(e.what());
        exit(1);
    }

    server.stop();

    std::cout << "bye!" << std::endl;
    exit(EXIT_SUCCESS);

};
