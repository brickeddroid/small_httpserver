#ifndef HTTPSERVER_HPP
#define HTTPSERVER_HPP

#include <httpmessage.hpp>

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <future>
#include <chrono>
#include <cstring>
#include <map>
#ifdef _WIN32
#include <WS2tcpip.h>
#else
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#ifdef __unix__
typedef int SOCKET;
#define SOCKET_ERROR -1
#define INVALID_SOCKET -1
#else
#pragma comment(lib, "ws2_32.lib")
#endif


static constexpr const unsigned int PORT = 5000;
static constexpr const unsigned int CLIENTS_MAX_NUM = 5;
static constexpr const unsigned int CLIENTS_QUEUE_NUM = 0;


using HttpUriHandler_t = std::function<void(const HttpRequest&, HttpResponse&)>;

class HttpServer {
public:
    HttpServer(const std::string host, std::uint16_t port);
    ~HttpServer();


    void register_uri_handler(const std::string& uri, HttpMethod method, const HttpUriHandler_t uri_handler);

    void start();
    void resume();
    void stop();

    //void add_handler(HttpHandler* const server_handler);

private:
    std::string m_host;
    std::uint16_t m_port;
    bool m_accepting;

    SOCKET m_socket;

    std::map<std::string, std::map<HttpMethod, HttpUriHandler_t>> m_uri_handlers;
    std::vector<std::future<void>> m_clients;

    void handle_client(SOCKET client_sock, sockaddr_in client);

#ifdef _WIN32
    void initialize_socket();
#endif
    void bind_socket();
    void open_socket();
    void close_socket();
    void listen_socket();

    void add_client(SOCKET client_id, sockaddr_in& client_addr);
    void clear_clients();
};

#endif
