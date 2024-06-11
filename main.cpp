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
#include "scripts/testing_threads.h"


void initializeLogging() {
    // Set the custom logger
    logger::DEFAULT_LOGGER = std::make_unique<logger::CustomLogger>(logger::Level::DEBUG);
}


int _main(int argc, char** argv) {
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
        {"lab-machine", required_argument, 0, 'l'},
        {0, 0, 0, 0}
    };

    std::string url;
    int threads = 8;
    std::string output = "found_urls.txt";
    size_t max_urls = 0;
    std::chrono::seconds max_time = std::chrono::seconds(0);
    bool debug = false;
    int cache_type = 1;
    bool lab_machine = false;

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
            case 'l':
                lab_machine = true;
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
    // std::string url1 = "https://www.google.com";
    // std::string url2 = "https://www.yahoo.com";
    // std::string url3 = "https://www.isnowfy.com/libcurl-fetch-webpage/";
    // std::string url4 = "https://www.example.com";
    // std::string url5 = "https://assets.cnblogs.com/logo.svg";
    // std::string url6 = "https://en.wikipedia.org/wiki/Main_Page";

    // Sleep for 5 seconds
    logger::info() << "\033[32mThe Crawler will start in 5 secs\033[0m" << logger::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));

    auto start = std::chrono::high_resolution_clock::now();

    Crawler crawler(url, threads, output, max_urls, max_time, lab_machine);
    crawler.start();

    

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    logger::info() << "\033[32mTime taken: " << elapsed.count() << " seconds\033[0m" << logger::endl;

    return 0;

}

int test_thread() {
    initializeLogging();

    //print all args
    logger::info() << "Starting testing for different threads" << logger::endl;

    

    std::string url = "https://en.wikipedia.org/wiki/Main_Page";
    std::string output = "found_urls.txt";
    size_t max_urls = 0;
    std::chrono::seconds max_time = std::chrono::seconds(300);
    bool debug = false;
    int cache_type = 0;
    int lab_machine = 1;

    //open a file for saving the results

    std::ofstream outputFile("test_results.txt", std::ios::out);
    if (!outputFile.is_open()) {
        logger::error() << "Failed to open the output file" << logger::endl;
        return 1;
    }

    outputFile << "Threads VisitedURLs FoundURLs Time" << std::endl;


    curl_global_init(CURL_GLOBAL_ALL);

    // for loop in [1,2,4,8,16,32,64]
    for (int i = 1; i <= 64; i *= 2) {
        logger::info() << "Number of threads: " << i << logger::endl;
        auto start = std::chrono::high_resolution_clock::now();

        Crawler crawler(url, i, output, max_urls, max_time, lab_machine);
        crawler.start();

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;

        int num_visited_urls = crawler.num_visited_urls;
        int num_found_urls = crawler.num_found_urls;

        logger::info() << "Time taken: " << elapsed.count() << " seconds" << logger::endl;
        outputFile << i << " " << num_visited_urls << " " << num_found_urls << " " << elapsed.count() << std::endl;
        logger::info() << "Sleep for 2 mins"<< logger::endl;
        std::this_thread::sleep_for(std::chrono::seconds(180));
    }
    return 0;


}


int main(int argc, char** argv) {
    return _main(argc, argv); // The Cralwer
    // return test_thread(); // The test for different threads
}