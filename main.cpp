#include <chrono>


#include "src/UrlFetcher.h"
#include "src/LinkExtractor.h"
#include "src/HttpResponse.h"
#include "src/LinkExtractor.h"
#include "src/HttpResponse.h"


#include "scripts/package_test.h"


int main() {
    std::string url1 = "https://www.google.com";
    std::string url2 = "https://www.yahoo.com";
    std::string url3 = "https://www.isnowfy.com/libcurl-fetch-webpage/";
    std::string url4 = "https://www.example.com";
    std::string url5 = "https://assets.cnblogs.com/logo.svg";

    auto start = std::chrono::high_resolution_clock::now();

    UrlFetcher fetcher = UrlFetcher();
    HttpResponse response;
    fetcher.fetch_url(url3, response);
    std::cout << "URL: " << response.url << std::endl;
    std::cout << "Status code: " << response.status_code << std::endl;
    std::cout << "Content type: " << response.content_type << std::endl;
    std::cout << "Charset: " << response.charset << std::endl;
    std::cout << "Base URL: " << response.base_url << std::endl;
    std::cout << "Domain: " << response.domain << std::endl;
    // std::cout << "HTML content: " << response.html_content << std::endl;

    LinkExtractor extractor = LinkExtractor();
    std::unordered_set<std::string> links;
    extractor.extract_links(response.html_content, links, response.base_url);
    for (const auto& link : links) {
        std::cout << link << std::endl;
    }
    

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "Elapsed time: " << elapsed.count() << " seconds" << std::endl;

}