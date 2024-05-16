#include <chrono>


#include "src/UrlFetcher.h"
#include "src/LinkExtractor.h"
#include "src/HttpResponse.h"
#include "src/LinkExtractor.h"
#include "src/HttpResponse.h"
#include "src/UrlStore.h"
#include "src/UrlScheduler.h"
#include "src/Crawler.h"


#include "scripts/package_test.h"





int main() {
    std::string url1 = "https://www.google.com";
    std::string url2 = "https://www.yahoo.com";
    std::string url3 = "https://www.isnowfy.com/libcurl-fetch-webpage/";
    std::string url4 = "https://www.example.com";
    std::string url5 = "https://assets.cnblogs.com/logo.svg";
    std::string url6 = "https://en.wikipedia.org/wiki/Main_Page";

    auto start = std::chrono::high_resolution_clock::now();

    Crawler crawler(url6, 8, "output.txt");
    crawler.start();

    


    

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "Elapsed time: " << elapsed.count() << " seconds" << std::endl;

}