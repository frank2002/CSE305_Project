#ifndef URLFETCHER_H
#define URLFETCHER_H

#include <curl/curl.h>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

#include "HttpResponse.h"

// size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp);

class UrlFetcher
{
public:
    UrlFetcher();
    ~UrlFetcher();
    int fetch_url(const std::string &url, HttpResponse &response, bool lab_machine);

private:
    CURL *curl;
    std::string UA = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.3";
    static size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp);
};

#endif // URLFETCHER_H