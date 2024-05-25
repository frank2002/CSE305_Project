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

// Function to fetch URL and populate HttpResponse structure
int UrlFetcher::fetch_url(const std::string& url, HttpResponse& response) {
    if (curl) {
        response.url = url;

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response.html_content);


        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "[ERROR] Fetching URL failed: " << curl_easy_strerror(res) << "; URL: "<< url << std::endl;
            return 1;
        }

        // Get the response code
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);

        // Get the content type
        char* content_type;
        curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &content_type);
        if (content_type) {
            response.content_type = content_type;

            // Optionally parse charset from content type if present
            std::string content_type_str(content_type);
            size_t pos = content_type_str.find("charset=");
            if (pos != std::string::npos) {
                response.charset = content_type_str.substr(pos + 8);
            }
        }

        // Extract base URL and domain
        size_t scheme_end = url.find("://");
        size_t path_start = url.find('/', scheme_end + 3);
        response.base_url = url.substr(0, path_start);
        response.domain = url.substr(scheme_end + 3, path_start - scheme_end - 3);
    } else {
        std::cerr << "[ERROR] Failed to initialize CURL" << std::endl;
        return 1;
    }

    return 0;
}