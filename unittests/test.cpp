#include <utpp/utpp.h>
#include <enumstringifier.hpp>
#include <httpmessage.hpp>
#include <httpconstants.hpp>
#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;
/*
bool earth_is_round ();
double earth_radius_km ();

TEST (EarthShape)
{
  CHECK (earth_is_round ());
}

TEST (HowBigIsEarth)
{
  CHECK_CLOSE (6371., earth_radius_km(), 1.);
}
*/
using namespace Http;

SUITE (EnumStringificationTests)
{

//TEST
TEST (EnumToString){
    CHECK_EQUAL("GET", Method::to_string(HttpMethod::GET));
}

//TEST
TEST (EnumFromString){
    CHECK_EQUAL(HttpMethod::GET, Method::from_string("GET"));
}

//TEST
TEST (EnumFromStringThrow){
    CHECK_THROW(std::runtime_error, Method::from_string("NOT A VALID HTTP METHOD"));
}

} // endof SUITE(EnumStringificationTests)

SUITE(HttpMessageTests){
//TEST
TEST (RequestHeaderFromString){
    HttpRequest  http_request;
    http_request.from_string("GET /index.html HTTP/1.1\r\nAccept: text/html\r\n\r\n");

    CHECK_EQUAL(HttpMethod::GET, http_request.method());
    CHECK_EQUAL("/index.html", http_request.path());
    CHECK_EQUAL(HttpVersion::Http_11, http_request.version());
    CHECK_EQUAL("text/html", http_request.header("Accept"));
}

//TEST
TEST (ResponseHeaderToString){
    HttpResponse http_response;
    http_response.set_version(HttpVersion::Http_20);
    http_response.set_status_code(HttpStatusCode::Ok);
    const std::string expected_response = "HTTP/2.0 200 Ok\r\n\r\n";

    CHECK_EQUAL(expected_response, http_response.to_string());
}
} // endof SUITE(HttpMessageTests)

#include <fstream>

std::string create_report_directory(){
    fs::path testreports = fs::current_path();
    time_t t = time(0);
    struct tm* now = localtime( &t );
    char dirname[60];
    strftime(dirname, 60, "Report_%Y%m%d_%H%M%S", now);
    if(!fs::exists(testreports / "Testreports")){
        std::cout << "Create Testreports dir here: " << testreports << std::endl;
        fs::create_directories(testreports / "Testreports");
    }
    std::cout << "Create " << testreports << "/Testreports/" << dirname << " directory" << std::endl;
    fs::create_directories(testreports / "Testreports" / dirname);
    return std::string(dirname);
}

TEST_MAIN (int argc, char** argv)
{
    std::string report_name = "OverallTests";
    if(argc >= 2){
        report_name = argv[1];
    }
    const std::string report_path = "Testreports/" + create_report_directory() + "/" + report_name + ".xml";
    std::ofstream os (report_path);
    UnitTest::ReporterXml xml (os);
    std::cout << argv[0] << " " << argc << std::endl;
    if(argc < 2){
        return UnitTest::RunAllTests (xml);
    }

    if(strcmp(argv[1], "EnumStringificationTests") == 0){
        std::cout << "Start running EnumStringification unit tests" << std::endl;
        return UnitTest::RunSuite("EnumStringificationTests", xml);
    } else if(strcmp(argv[1], "HttpMessageTests") == 0){
        std::cout << "Start running HttpMessage unit tests" << std::endl;
        return UnitTest::RunSuite("HttpMessageTests", xml);
    }
}
