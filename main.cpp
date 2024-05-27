#define CPPLOG_NAMESPACE logger


#include <chrono>
#include <cpplog/log.h>
#include <cpplog/logger.h>
#include <getopt.h>



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


int main(int argc, char** argv) {
    initializeLogging();

    //print all args
    logger::info() << "Arguments: ";
    for (int i = 0; i < argc; i++) {
        logger::info() << argv[i] << " " << logger::endl;
    }

    static struct option long_options[] = {
        {"url", required_argument, 0, 'u'},
        {"threads", required_argument, 0, 't'},
        {"output", required_argument, 0, 'o'},
        {"max-urls", required_argument, 0, 'm'},
        {"max-time", required_argument, 0, 's'},
        {"debug", required_argument, 0, 'd'},
        {"cache-type", required_argument    , 0, 'c'},
        {0, 0, 0, 0}
    };

    std::string url;
    int threads = 8;
    std::string output = "found_urls.txt";
    size_t max_urls = 0;
    std::chrono::seconds max_time = std::chrono::seconds(0);
    bool debug = false;
    int cache_type = 1;

    int opt_index = 0;
    int c;
    while ((c = getopt_long(argc, argv, "u:t:o:m:s:d:c:", long_options, &opt_index)) != -1) {
        switch (c) {
            case 'u':
                url = optarg;
                break;
            case 't':
                threads = std::stoi(optarg);
                break;
            case 'o':
                output = optarg;
                break;
            case 'm':
                max_urls = std::stoi(optarg);
                break;
            case 's':
                max_time = std::chrono::seconds(std::stoi(optarg));
                break;
            case 'd':
                debug = true;
                break;
            case 'c':
                cache_type = std::stoi(optarg);
                break;
            default:
                break;
        }
    }

    if (url.empty()) {
        logger::error() << "Please provide a URL" << logger::endl;
        return 1;
    }

    if (threads <= 0) {
        logger::error() << "Number of threads should be greater than 0" << logger::endl;
        return 1;
    }

    if (max_urls < 0) {
        logger::error() << "Maximum number of URLs should be greater than or equal to 0" << logger::endl;
        return 1;
    }

    if (max_time.count() < 0) {
        logger::error() << "Maximum time should be greater than or equal to 0" << logger::endl;
        return 1;
    }

    if (cache_type != 0 && cache_type != 1) {
        logger::error() << "Cache type should be 0 or 1" << logger::endl;
        return 1;
    }

    if (debug) {
        logger::DEFAULT_LOGGER = std::make_unique<logger::CustomLogger>(logger::Level::DEBUG);
    } else {
        logger::DEFAULT_LOGGER = std::make_unique<logger::CustomLogger>(logger::Level::INFO);
    }


    curl_global_init(CURL_GLOBAL_ALL);
    std::string url1 = "https://www.google.com";
    std::string url2 = "https://www.yahoo.com";
    std::string url3 = "https://www.isnowfy.com/libcurl-fetch-webpage/";
    std::string url4 = "https://www.example.com";
    std::string url5 = "https://assets.cnblogs.com/logo.svg";
    std::string url6 = "https://en.wikipedia.org/wiki/Main_Page";

    auto start = std::chrono::high_resolution_clock::now();

    Crawler crawler(url, threads, output, max_urls, max_time);
    crawler.start();

    


    

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    logger::info() << "\033[32mTime taken: " << elapsed.count() << " seconds\033[0m" << logger::endl;

}