#include <httpserver.hpp>
#include <httpconstants.hpp>

#include <stdexcept>

HttpServer::HttpServer(const std::string host, std::uint16_t port)
    : m_host(host),
      m_port(port)
{
}

HttpServer::~HttpServer() {
#ifdef _WIN32
        WSACleanup();
#endif
}


void HttpServer::start()
{
#ifdef _WIN32
    initialize_socket();
#endif
    resume();
}

void HttpServer::resume(){
    open_socket();
    bind_socket();
    listen_socket();
}

void HttpServer::stop()
{
    close_socket();
}

void HttpServer::register_uri_handler(const std::string& uri, HttpMethod method, const HttpUriHandler_t uri_handler){
    m_uri_handlers[uri].insert(std::make_pair(method, std::move(uri_handler)));
}


void HttpServer::handle_client(SOCKET client_sock, sockaddr_in client) {

    char buf[4096];
    char host[NI_MAXHOST];
    char service[NI_MAXHOST];

    memset(host, 0, NI_MAXHOST);
    memset(service, 0, NI_MAXHOST);

    //std::cout << std::this_thread::get_id() << std::endl;

    if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {

        std::cout << host << " connected on port " << service << std::endl;

    }
    else {

        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);

        std::cout << host << " connected on port " << ntohs(client.sin_port) << std::endl;

    }

    while (true) {

        memset(&buf, 0, 4096);

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
            break;
        }
        else {
            std::cout << "send to client" << std::endl;
            std::ostringstream oss;
            HttpRequest http_request;
            HttpResponse http_response;
            http_request.from_string(std::string(buf));
            int bytes_sent;

            if(!http_request.is_valid()){
                http_response.set_status_code(HttpStatusCode::BadRequest);
                std::string response = http_response.to_string();
                bytes_sent = send(client_sock, response.c_str(), response.length(), 0);
                std::cout << "sent " << bytes_sent << " bytes" << std::endl << response << std::endl;
                continue;
            }

            auto it = m_uri_handlers.find(http_request.path());
            if(it == m_uri_handlers.end()){
                http_response.set_status_code(HttpStatusCode::NotFound);
                http_response.add_header("Content-Length", "0");
                std::string response = http_response.to_string();
                bytes_sent = send(client_sock, response.c_str(), response.length(), 0);
                std::cout << "Path not registered " << http_request.path() << std::endl;
                std::cout << "sent " << bytes_sent << " bytes| length " << response.length() << "bytes| body:\n" << response << std::endl;
                continue;
            }
            auto cit = it->second.find(http_request.method());
            if(cit == it->second.end()){
                std::cout << "Method not registered " << method_to_string(http_request.method()) << std::endl;
                http_response.set_status_code(HttpStatusCode::MethodNotAllowed);
                http_response.set_version(HttpVersion::Http_11);
                std::string response = http_response.to_string();
                bytes_sent = send(client_sock, response.c_str(), response.length(), 0);
                continue;
            }

            cit->second(http_request, http_response);

            const std::string& response = http_response.to_string();
            bytes_sent = send(client_sock, response.c_str(), response.length(), 0);
            std::cout << "sent " << bytes_sent << " bytes| length " << response.length() << std::endl << response << std::endl;
        }

    }

}

#ifdef _WIN32
void HttpServer::initialize_socket() {

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

void HttpServer::bind_socket() {

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
}

void HttpServer::open_socket() {

    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

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
}
void HttpServer::close_socket() {

    std::cout << "closing server socket" << std::endl;
#ifdef _WIN32
    closesocket(m_socket);
#else
    close(m_socket);
#endif
    m_accepting = false;
}

void HttpServer::clear_clients() {

    std::vector<std::future<void>>::iterator it = m_clients.begin();

    while (it != m_clients.end()) {
        auto status = (*it).wait_for(std::chrono::milliseconds(0));
        if (status == std::future_status::ready) {
            it = m_clients.erase(it);
        }
        else {
            ++it;
        }
    }
}

void HttpServer::add_client(SOCKET client_socket, sockaddr_in& client){
    m_clients.emplace_back(std::async(std::launch::async, &HttpServer::handle_client, this, client_socket, client));
    if (m_clients.size() >= CLIENTS_MAX_NUM && m_accepting) {
        stop();
    }
    std::cout << m_clients.size() << " clients connected" << std::endl;
}

void HttpServer::listen_socket() {

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

        while (true) {

            if (m_clients.size() < CLIENTS_MAX_NUM && !m_accepting) {

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

                if (client_sock == INVALID_SOCKET) {


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

                //clear_clients(m_clients);
                add_client(client_sock, client);

            }
        }
    }
}
