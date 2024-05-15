#include "UrlFetcher.h"

size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

UrlFetcher::UrlFetcher() {
    curl = curl_easy_init();
}

UrlFetcher::~UrlFetcher() {
    curl_easy_cleanup(curl);
}

int UrlFetcher::fetch_url(const std::string& url, std::string& html_content) {
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &html_content);
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            return 1;
        }
    } else {
        std::cerr << "Failed to initialize CURL" << std::endl;
        return 1;
    }
    return 0;
}