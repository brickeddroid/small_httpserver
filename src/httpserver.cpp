#include "../include/httpserver.hpp"

#include "../include/httpconstants.hpp"

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

#ifdef _WIN32
void HttpServer::initialize_socket() {

    WSAData wsData;
    WORD ver = MAKEWORD(2, 2);

    int wsResult = WSAStartup(ver, &wsData);

    if (wsResult != 0) {

        std::cerr << WSAGetLastError() << std::endl;

        WSACleanup();
        exit(EXIT_FAILURE);

    }

}
#else
void HttpServer::initialize_socket(){
    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socket < 0)
    {
        std::cerr << "Initialize server socket failed" << std::endl;
    }
}
#endif

void HttpServer::register_uri_handler(std::string uri, HttpMethod method, const HttpUriHandler_t uri_handler){
    //m_uri_handlers[uri][method] = uri_handler;
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
#else
        if (bytes_received < 0) {
            exitWithError("SOCKET ERROR");
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
            http_response.add_header("Content-Length", "0");
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
                http_response.set_version(HttpVersion::Http_10);
                std::string response = http_response.to_string();
                bytes_sent = send(client_sock, response.c_str(), response.length(), 0);
                continue;
            }

            http_response = cit->second(http_request);

            const std::string& response = http_response.to_string();
            bytes_sent = send(client_sock, response.c_str(), response.length(), 0);
            std::cout << "sent " << bytes_sent << " bytes| length " << response.length() << std::endl << response << std::endl;

        }

    }

}

void HttpServer::bind_socket(SOCKET &server_sock) {

    int keep_alive = 1;
    int re_use = 1;
#ifdef _WIN32
    if (setsockopt(server_sock, SOL_SOCKET, SO_KEEPALIVE, (const char*)&keep_alive, sizeof(keep_alive)) == SOCKET_ERROR) {

        closesocket(server_sock);
        WSACleanup();
        exit(EXIT_FAILURE);

    }

    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&re_use, sizeof(re_use)) == SOCKET_ERROR) {

        closesocket(server_sock);
        WSACleanup();
        exit(EXIT_FAILURE);

    }
#else
    if (setsockopt(server_sock, SOL_SOCKET, SO_KEEPALIVE, (const char*)&keep_alive, sizeof(keep_alive)) == -1) {

        close(server_sock);
        exit(EXIT_FAILURE);

    }

    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&re_use, sizeof(re_use)) == -1) {

        close(server_sock);
        exit(EXIT_FAILURE);

    }
#endif

    sockaddr_in server;

    server.sin_family = AF_INET;
    server.sin_port = htons(m_port);
    inet_pton(AF_INET, m_host.c_str(), &(server.sin_addr.s_addr));
#ifdef _WIN32
    server.sin_addr.S_un.S_addr = INADDR_ANY;
#else
    server.sin_addr.s_addr = INADDR_ANY;
#endif
    memset(&server.sin_zero, 0, 8);
#ifdef _WIN32
    if (bind(server_sock, (sockaddr*)&server, sizeof(sockaddr)) == SOCKET_ERROR) {
        std::cerr << WSAGetLastError() << std::endl;
        closesocket(server_sock);
        WSACleanup();
        exit(EXIT_FAILURE);
    }
#else
    if (bind(server_sock, (sockaddr*)&server, sizeof(sockaddr)) == -1) {
        std::cerr << "SOCKET ERROR" << std::endl;
        close(server_sock);
        exit(EXIT_FAILURE);
    }
#endif
}

void HttpServer::open_socket(SOCKET &server_sock, bool &accepting) {

    server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

#ifdef _WIN32
    if (server_sock == INVALID_SOCKET) {
        std::cerr << WSAGetLastError() << std::endl;
        WSACleanup();
#else
    if (server_sock < 0) {
        std::cerr << "INVALID SOCKET" << std::endl;
#endif

        accepting = false;
        exit(EXIT_FAILURE);

    }

    accepting = true;
}
void HttpServer::close_socket(SOCKET &server_sock, bool &accepting) {

#ifdef _WIN32
    closesocket(server_sock);
#else
    close(server_sock);
#endif

    accepting = false;

}

void HttpServer::clear_futures(std::vector<std::future<void>> &futures) {

    std::vector<std::future<void>>::iterator it = futures.begin();

    while (it != futures.end()) {
        auto status = (*it).wait_for(std::chrono::milliseconds(0));
        if (status == std::future_status::ready) {
            it = futures.erase(it);
        }
        else {
            ++it;
        }
    }
}

void HttpServer::listen_socket(std::vector<std::future<void>> &futures, SOCKET &server_sock, bool& accepting) {

    if (listen(server_sock, CLIENTS_QUEUE_NUM) == SOCKET_ERROR) {
#ifdef _WIN32
        std::cerr << WSAGetLastError() << std::endl;
        closesocket(server_sock);
        WSACleanup();
#else
        std::cerr << "SOCKET_ERROR" << std::endl;
        close(server_sock);
#endif
        exit(EXIT_FAILURE);

    }
    else {

        std::cout << "accepting for incoming connections on port " << m_port << std::endl;

        while (true) {

            if (futures.size() < CLIENTS_MAX_NUM && !accepting) {

                std::cout << "re-opening server socket" << std::endl;

                open_socket(server_sock, accepting);
                bind_socket(server_sock);
                listen_socket(futures, server_sock, accepting);

                break;

            }

            if (accepting) {

                sockaddr_in client;

                memset(&client.sin_zero, 0, 8);
#ifdef _WIN32
                int client_size = sizeof(client);
#else
                unsigned int client_size = sizeof(client);
#endif

                SOCKET client_sock = accept(server_sock, (sockaddr*)&client, &client_size);

                if (client_sock == INVALID_SOCKET) {


#ifdef _WIN32
                    std::cerr << WSAGetLastError() << std::endl;
                    closesocket(server_sock);
                    WSACleanup();
#else
                    std::cerr << "INVALID SOCKET" << std::endl;
#endif
                    exit(EXIT_FAILURE);

                }

                clear_futures(futures);

                futures.emplace_back(std::async(std::launch::async, &HttpServer::handle_client, this, client_sock, client));

                if (futures.size() >= CLIENTS_MAX_NUM && accepting) {

                    std::cout << "closing server socket" << std::endl;

                    close_socket(server_sock, accepting);

                }

                std::cout << futures.size() << " clients connected" << std::endl;

            }

        }

    }

}
