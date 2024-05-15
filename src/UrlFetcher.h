#ifndef URLFETCHER_H
#define URLFETCHER_H

#include <curl/curl.h>
#include <iostream>
#include <string>


size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp);

class UrlFetcher {
public:
    UrlFetcher();
    ~UrlFetcher();
    int fetch_url(const std::string& url, std::string& html_content);

private:
    CURL* curl;
};

#endif // URLFETCHER_H