#ifndef HTTPMESSAGE_HPP
#define HTTPMESSAGE_HPP

#include "httpconstants.hpp"
#include <string>
#include <sstream>

void exitWithError(const char* msg);

class HttpMessage {
protected:
    std::string m_headline;
    std::map<std::string, std::string> m_headers;
    std::string m_content;
    HttpVersion m_version;
    HttpStatusCode m_status;
    HttpMethod m_method;
    std::string m_path;
public:
    HttpMessage();
    ~HttpMessage() = default;

    void set_version(HttpVersion version);
    HttpVersion version();

    void set_status_code(HttpStatusCode status);
    HttpStatusCode status_code();

    void add_header(const std::string& key, const std::string& value);
    std::string header(const std::string& key) const;
    void parse_header(const std::string& headers);

    void set_content(const std::string& content);
    const std::string& content();

    std::string to_string(bool include_content = true);
    void from_string(const std::string& http_message);

    virtual void parse_headline(const std::string& head_line) = 0;
    virtual std::string headline() = 0;
    virtual bool is_valid() {
        return true;
    }
};

class HttpRequest : public HttpMessage {
private:
    std::map<std::string, std::string> m_query_parameters;

    void add_query_parameter(const std::string& key, const std::string& value);

    virtual void parse_headline(const std::string& head_line);
    virtual std::string headline();
public:
    HttpRequest();
    ~HttpRequest() = default;
    const std::string& path() const;
    const HttpMethod& method() const;
    std::string query_parameter(const std::string& key) const;
};

class HttpResponse : public HttpMessage {
private:
    virtual void parse_headline(const std::string& head_line);
    virtual std::string headline();
public:
    HttpResponse();
    ~HttpResponse() = default;
};

#endif
