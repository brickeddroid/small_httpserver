#ifndef HTTPSERVER_HPP
#define HTTPSERVER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <future>
#include <thread>
#include <chrono>
//#include <netinet/in.h>
#include <mutex>
#include <unordered_set>
#include <map>

#include <fstream>
#include <cstring>
#include <queue>
#ifdef _WIN32
#include <WS2tcpip.h>
#else
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#define SOCKET_ERROR -1
typedef int SOCKET;
#endif

#include "httpmessage.hpp"

namespace Http {

static constexpr const unsigned int PORT = 5000;
static constexpr const unsigned int CLIENTS_MAX_NUM = 5;
static constexpr const unsigned int CLIENTS_QUEUE_NUM = 0;

using UriHandlerCallback_t = std::function<void(const HttpRequest&, HttpResponse&)>;

void info(const char* msg);
void error(const char* msg);


class EventSource {
public:
    EventSource() {}

    void send_event(const std::string& event);
    void add_client(int clientSocket);
    void remove_client(int clientSocket);
    void remove_clients();
    void close_connection();

    void onConnect(SOCKET client_sock);
    void onClose(SOCKET client_sock);

private:
    std::unordered_set<int> m_client_sockets;
    std::mutex m_mutex;
    std::vector<std::future<void>> m_message_queue;

    void clear_message_queue();
    void send_future_event(const std::string& str);
};

struct Client {
    int socket;
    sockaddr_in address;
};

class Server {
public:
    enum Status {
        STOPPED,
        STARTING,
        RUNNING,
        STOPPING
    };

    Server(const std::string host, int port);

    Status status() { return m_status.load(); }
    void start();
    void stop();
    void register_event(const std::string& uri, EventSource* eventSource);
    void register_uri_handler(const std::string& uri, HttpMethod method, const UriHandlerCallback_t cb);

    void onNotFound(const UriHandlerCallback_t onNotFoundCb) { onNotFoundCallback = std::move(onNotFoundCb); }

private:
    SOCKET m_socket;
    std::atomic<Status> m_status;
    const std::string m_host;
    int m_port;
    //std::atomic<bool> m_stop = false;
    bool m_accepting = true;
    std::thread m_serverThread;
    std::map<std::string, EventSource*> m_eventSource;
    std::map<std::string, std::map<HttpMethod, UriHandlerCallback_t>> m_uri_handlers;
    std::vector<std::future<void>> m_client_requests;

    //void start_server();
    void handle_connection(int clientSocket);

    void start_server();
    void open_socket();
    void bind_socket();
    void listen_socket();
    void close_socket();

    void add_client(const Client& http_client);
    void clear_clients();
    void handle_client(const Client& http_client);

    UriHandlerCallback_t onNotFoundCallback;
    static void onNotFoundDefaultCallback(const HttpRequest& request, HttpResponse& response);


};

} // end namespace Http

#endif


