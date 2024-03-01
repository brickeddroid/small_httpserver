#ifndef HTTPSERVER_HPP
#define HTTPSERVER_HPP

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

#include "httpmessage.hpp"


static constexpr const unsigned int PORT = 5000;
static constexpr const unsigned int CLIENTS_MAX_NUM = 5;
static constexpr const unsigned int CLIENTS_QUEUE_NUM = 0;


//typedef std::string Uri_t;
using HttpUriHandler_t = std::function<HttpResponse(const HttpRequest&)>;

class HttpServer {
public:
    HttpServer(const std::string host, std::uint16_t port);
    ~HttpServer();

    void initialize_socket();

    void register_uri_handler(std::string uri, HttpMethod method, const HttpUriHandler_t uri_handler);

    void handle_client(SOCKET client_sock, sockaddr_in client);

    void bind_socket(SOCKET &server_sock);

    void open_socket(SOCKET &server_sock, bool &accepting);

    void close_socket(SOCKET &server_sock, bool &accepting);

    void clear_futures(std::vector<std::future<void>> &futures);

    void listen_socket(std::vector<std::future<void>> &futures, SOCKET &server_sock, bool& accepting);

private:
    std::string m_host;
    std::uint16_t m_port;

    SOCKET m_socket;

    std::map<std::string, std::map<HttpMethod, HttpUriHandler_t>> m_uri_handlers;
};

#endif
