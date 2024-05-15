#include <chrono>


#include "src/UrlFetcher.h"


#include "scripts/package_test.h"


int main() {
    std::string url1 = "https://www.google.com";
    std::string url2 = "https://www.yahoo.com";
    std::string url3 = "https://www.bing.com";
    std::string url4 = "https://www.example.com";

    auto start = std::chrono::high_resolution_clock::now();

    UrlFetcher fetcher = UrlFetcher();
    std::string html_content;
    fetcher.fetch_url(url4, html_content);
    std::cout << html_content << std::endl;

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "Elapsed time: " << elapsed.count() << " seconds" << std::endl;

}