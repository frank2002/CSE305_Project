#define CPPLOG_NAMESPACE logger


#include <chrono>
#include <cpplog/log.h>
#include <cpplog/logger.h>


#include "src/UrlFetcher.h"
#include "src/LinkExtractor.h"
#include "src/HttpResponse.h"
#include "src/LinkExtractor.h"
#include "src/HttpResponse.h"
#include "src/UrlStore.h"
#include "src/UrlScheduler.h"
#include "src/Crawler.h"
// #include "CustomLogger.h"



#include "scripts/package_test.h"


void initializeLogging() {
    // Set the custom logger
    logger::DEFAULT_LOGGER = std::make_unique<logger::CustomLogger>(logger::Level::DEBUG);
}


int main() {
    initializeLogging();

    curl_global_init(CURL_GLOBAL_ALL);
    std::string url1 = "https://www.google.com";
    std::string url2 = "https://www.yahoo.com";
    std::string url3 = "https://www.isnowfy.com/libcurl-fetch-webpage/";
    std::string url4 = "https://www.example.com";
    std::string url5 = "https://assets.cnblogs.com/logo.svg";
    std::string url6 = "https://en.wikipedia.org/wiki/Main_Page";

    auto start = std::chrono::high_resolution_clock::now();

    Crawler crawler(url6, 10, "output.txt");
    crawler.start();

    


    

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "Elapsed time: " << elapsed.count() << " seconds" << std::endl;

}