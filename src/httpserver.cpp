#include "../include/httpserver.hpp"
#include "../include/httpmessage.hpp"

namespace Http {

void info(const char* msg){
    std::cout << msg << std::endl;
}

void error(const char* msg){
    std::cerr << msg << std::endl;
}



void EventSource::send_event(const std::string& event) {
    static int count = 0;
    ++count;
    std::cout << "Called " << count << " times to add request" << std::endl;
    m_message_queue.emplace_back(std::async(std::launch::async, &EventSource::send_future_event, this, event));
    clear_message_queue();
    std::cout << m_message_queue.size() << " messages in queue" << std::endl;

    //info("Free sendEvent Client");
}

void EventSource::close_connection(){
    // Ugly workaround
    std::string close_str = "data: close\n\n";

    info("Lock closeEvent connection");
    std::lock_guard<std::mutex> lock(m_mutex);
    for (int client_socket : m_client_sockets) {
        int bytesSent = send(client_socket, close_str.c_str(), close_str.size(), 0);
        if (bytesSent != close_str.size()) {
            error("Error sending event to client");
        } else {
            info("Connection closed");
        }
    }
    info("Free closeEvent connection");
}

void EventSource::add_client(int client_socket) {
    info("Lock Add Client");
    std::lock_guard<std::mutex> lock(m_mutex);
    m_client_sockets.insert(client_socket);
    onConnect(client_socket);
    info("Free Add Client");
}

void EventSource::remove_client(int client_socket) {
    info("Lock Remove Client");
    std::lock_guard<std::mutex> lock(m_mutex);
    m_client_sockets.erase(client_socket);
    onClose(client_socket);
    info("Free Remove Client");
}

void EventSource::remove_clients(){
    info("Lock Remove Clients");
    std::lock_guard<std::mutex> lock(m_mutex);
    m_client_sockets.clear();
    info("Free Remove Clients");
}

void EventSource::clear_message_queue(){
    info("Clear message queue");
    std::vector<std::future<void>>::iterator it = m_message_queue.begin();

    while (it != m_message_queue.end()) {
        auto status = (*it).wait_for(std::chrono::milliseconds(0));
        if (status == std::future_status::ready) {
            it = m_message_queue.erase(it);
        }
        else {
            info("Not ready yet");
            ++it;
        }
    }
    info("cleared");
}

void EventSource::send_future_event(const std::string& event){
    std::lock_guard<std::mutex> lock(m_mutex);
    for (int client_socket : m_client_sockets) {
        int bytesSent = send(client_socket, event.c_str(), event.size(), 0);
        if (bytesSent != event.size()) {
            error("Error sending event to client");
        } else {
            //info("Sent event");
        }
    }
}

void EventSource::onConnect(SOCKET client_socket){
    HttpResponse http_response;
    http_response.set_status_code(HttpStatusCode::Ok);
    http_response.set_version(HttpVersion::Http_11);
    http_response.add_header("Cache-Control", "no-cache");
    http_response.add_header("Content-Type", "text/event-stream");
    //http_response.add_header("Content-Length", "0");
    http_response.add_header("Connection", "keep-alive");
    std::string response = http_response.to_string();
    send(client_socket, response.c_str(), response.length(), 0);
}

void EventSource::onClose(SOCKET client_socket){
    //http_response.set_status_code(HttpStatusCode::Ok);
    //http_response.set_version(HttpVersion::Http_11);
    //http_response.add_header("Cache-Control", "no-cache");
    //http_response.add_header("Content-Type", "text/plain");
    //http_response.add_header("Content-Length", "0");
    //http_response.add_header("Connection", "keep-alive");
}

Server::Server(const std::string host, int port)
    : m_host(host),
      m_port(port),
      onNotFoundCallback(onNotFoundDefaultCallback)
{}

void Server::start() {

    //m_stop = false;
    m_status = Status::STARTING;
    m_serverThread = std::thread(&Server::start_server, this);
}

void Server::stop() {
    if(m_status != Status::RUNNING) return;
    m_status = Status::STOPPING;
    //m_stop = true;
    //close(serverSocket);
    for(auto es : m_eventSource){
        es.second->close_connection();
    }

    if (m_serverThread.joinable()) {
        info("stopping server...");
        m_serverThread.join();
        info("stopped");
    }
    m_status = Status::STOPPED;

}

void Server::register_event(const std::string& uri, EventSource* eventSource) {
    m_eventSource[uri] = eventSource;
}

void Server::register_uri_handler(const std::string& uri, HttpMethod method, const UriHandlerCallback_t cb){
    m_uri_handlers[uri].insert(std::make_pair(method, std::move(cb)));
}

void Server::start_server(){
    open_socket();
    bind_socket();
    listen_socket();
}

void Server::add_client(const Client& http_client){
    // TODO Add client_socket as target to optional event source
    //SOCKET client_socket = http_client.socket;
    //sockaddr_in client = http_client.address;
    static int count = 0;
    ++count;
    std::cout << "Called " << count << " times to add request" << std::endl;
    m_client_requests.emplace_back(std::async(std::launch::async, &Server::handle_client, this, http_client));
    if (m_client_requests.size() >= CLIENTS_MAX_NUM && m_accepting) {
        close_socket();
    }
    std::cout << m_client_requests.size() << " client(s) connected" << std::endl;
}

void Server::clear_clients() {
    info("Clear clients");
    std::vector<std::future<void>>::iterator it = m_client_requests.begin();

    while (it != m_client_requests.end()) {
        auto status = (*it).wait_for(std::chrono::milliseconds(0));
        if (status == std::future_status::ready) {
            it = m_client_requests.erase(it);
        }
        else {
            info("Not ready yet");
            ++it;
        }
    }
    info("cleared");
}

void Server::handle_client(const Client& http_client) {
    sockaddr_in client = http_client.address;
    int client_sock = http_client.socket;

    char buf[4096];
    char host[NI_MAXHOST];
    char service[NI_MAXHOST];

    memset(host, 0, NI_MAXHOST);
    memset(service, 0, NI_MAXHOST);

    if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {

        std::cout << host << " connected on port " << service << std::endl;

    }
    else {
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);

        std::cout << host << " connected on port " << ntohs(client.sin_port) << std::endl;
    }


    while (m_status == Status::RUNNING) {

        memset(&buf, 0, 4096);
        info("Receiving...");
        const int bytes_received = recv(client_sock, buf, 4096, 0);

#ifdef _WIN32
        if (bytes_received == SOCKET_ERROR) {
            std::cerr << WSAGetLastError() << std::endl;
            WSACleanup();
            throw std::runtime_error("SOCKET ERROR");
#else
        if (bytes_received < 0) {
            throw std::runtime_error("SOCKET ERROR");
#endif
        }
        else if (bytes_received == 0) {
            std::cout << "client disconnected" << std::endl;

            for(const auto& es : m_eventSource){
                es.second->remove_client(client_sock);
            }
            close(client_sock);
            info("client socket closed");
            break;
        }
        else {
            std::cout << "client message received" << std::endl;
            std::ostringstream oss;
            HttpRequest http_request;
            HttpResponse http_response;
            http_request.from_string(std::string(buf));
            int bytes_sent;
            auto it = m_eventSource.find(http_request.path());
            if (it != m_eventSource.end()) {
                info("Event registration detected");
                it->second->add_client(client_sock);
                //it->second->onConnect(client_sock);
                info("Event registration finished");
            } else {
                auto uit = m_uri_handlers.find(http_request.path());
                if(uit == m_uri_handlers.end()){
                    onNotFoundCallback(http_request, http_response);
                    std::string response = http_response.to_string();
                    send(client_sock, response.c_str(), response.length(), 0);
                    info("Uri not found/not registered");
                    info(http_request.path().c_str());
                    continue;
                }
                auto cb = uit->second.find(http_request.method());
                if(cb == uit->second.end()){
                    onNotFoundCallback(http_request, http_response);
                    std::string response = http_response.to_string();
                    send(client_sock, response.c_str(), response.length(), 0);
                    info("Method not registered for this uri");
                    continue;
                }
                cb->second(http_request, http_response);
                std::string response = http_response.to_string();
                send(client_sock, response.c_str(), response.length(), 0);
            }
        }
        info("Future loop");
    }
}

#ifdef _WIN32
void Server::initialize_socket() {

    WSAData wsData;
    WORD ver = MAKEWORD(2, 2);

    int wsResult = WSAStartup(ver, &wsData);

    if (wsResult != 0) {

        std::cerr << WSAGetLastError() << std::endl;

        WSACleanup();

        throw std::runtime_error("Socket initialization failed");
    }
}
#endif

void Server::bind_socket() {
    info("Binding server socket to address");

    int keep_alive = 1;
    int re_use = 1;

    SOCKET status_keep_alive = setsockopt(m_socket, SOL_SOCKET, SO_KEEPALIVE, (const char*)&keep_alive, sizeof(keep_alive));
    SOCKET status_reuse_addr = setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&re_use, sizeof(re_use));

    if ( status_keep_alive == SOCKET_ERROR ||
         status_reuse_addr == SOCKET_ERROR ){
        stop();
#ifdef _WIN32
        WSACleanup();
#endif
        throw std::runtime_error("SOCKET ERROR");
    }
    sockaddr_in server;

    server.sin_family = AF_INET;
    server.sin_port = htons(m_port);
    inet_pton(AF_INET, m_host.c_str(), &(server.sin_addr.s_addr));
    memset(&server.sin_zero, 0, 8);

#ifdef _WIN32
    server.sin_addr.S_un.S_addr = INADDR_ANY;
    if (bind(m_socket, (sockaddr*)&server, sizeof(sockaddr)) == SOCKET_ERROR) {
        std::cerr << WSAGetLastError() << std::endl;
        closesocket(m_socket);
        WSACleanup();
        throw std::runtime_error("SOCKET ERROR");
    }
#else
    server.sin_addr.s_addr = INADDR_ANY;
    if (bind(m_socket, (sockaddr*)&server, sizeof(sockaddr)) == -1) {
        close(m_socket);
        throw std::runtime_error("SOCKET ERROR");
    }
#endif
    info("bound");
}

void Server::open_socket() {
    info("Open server socket...");
    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    fcntl(m_socket, F_SETFL, O_NONBLOCK);

#ifdef _WIN32
    if (m_socket == INVALID_SOCKET) {
        std::cerr << WSAGetLastError() << std::endl;
        WSACleanup();
#else
    if (m_socket < 0) {
        std::cerr << "INVALID SOCKET" << std::endl;
#endif
        m_accepting = false;
        throw std::runtime_error("INVALID SOCKET");
    }

    m_accepting = true;
    info("opened");
}

void Server::close_socket() {

    std::cout << "closing server socket" << std::endl;
#ifdef _WIN32
    closesocket(m_socket);
#else
    close(m_socket);
#endif
    m_accepting = false;
    info("closed");
}

void Server::listen_socket() {
    info("Listening on server socket");

    if (listen(m_socket, CLIENTS_QUEUE_NUM) == SOCKET_ERROR) {
#ifdef _WIN32
        std::cerr << WSAGetLastError() << std::endl;
        closesocket(m_socket);
        WSACleanup();
#else
        //std::cerr << "SOCKET_ERROR" << std::endl;
        close(m_socket);
#endif
        throw std::runtime_error("SOCKET ERROR");

    }
    else {

        std::cout << "accepting for incoming connections on port " << m_port << std::endl;

        m_status = Status::RUNNING;
        while (m_status == Status::RUNNING) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));

            if (m_client_requests.size() < CLIENTS_MAX_NUM && !m_accepting) {

                std::cout << "re-opening server socket" << std::endl;
                //start();
                open_socket();
                bind_socket();
                listen_socket();

                break;

            }

            if (m_accepting) {

                sockaddr_in client;

                memset(&client.sin_zero, 0, 8);
#ifdef _WIN32
                int client_size = sizeof(client);
#else
                unsigned int client_size = sizeof(client);
#endif

                SOCKET client_sock = accept(m_socket, (sockaddr*)&client, &client_size);
                //fcntl(client_sock, F_SETFL, O_NONBLOCK);

                if (client_sock < 0) {
                    continue;
#ifdef _WIN32
                    std::cerr << WSAGetLastError() << std::endl;
                    closesocket(m_socket);
                    WSACleanup();
#else
                    std::cerr << "INVALID SOCKET, closing..." << std::endl;
                    close(m_socket);
#endif
                    throw std::runtime_error("INVALID SOCKET");

                }

                clear_clients();
                Client http_client(client_sock, client);
                add_client(http_client);
                //update();
            }
        }
        clear_clients();
        close_socket();
    }
}

void Server::onNotFoundDefaultCallback(const HttpRequest& http_request, HttpResponse& http_response){
    http_response.set_status_code(HttpStatusCode::NotFound);
    http_response.set_version(HttpVersion::Http_11);
    http_response.add_header("Content-Length", "0");
}

} // end namespace Http
