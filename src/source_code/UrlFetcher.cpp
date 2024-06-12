#define CPPLOG_NAMESPACE logger


#include "UrlFetcher.h"

#include <cpplog/log.h>




size_t UrlFetcher::writeCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t totalSize = size * nmemb;
    std::string* str = static_cast<std::string*>(userp);

    // Sanity check
    if (!str) {
        // std::cerr << "[ERROR] Null pointer received in writeCallback" << std::endl;
        logger::error() << "Null pointer received in writeCallback" << logger::endl;
        return 0;  // Returning 0 will signal CURL that an error occurred
    }

    try {
        str->append(static_cast<char*>(contents), totalSize);
    } catch (const std::exception& e) {
        // std::cerr << "[ERROR] Exception in writeCallback: " << e.what() << std::endl;
        logger::error() << "Exception in writeCallback: " << e.what() << logger::endl;
        return 0;
    }

    return totalSize;
}

UrlFetcher::UrlFetcher() {
    curl = curl_easy_init();
}

UrlFetcher::~UrlFetcher() {
    curl_easy_cleanup(curl);
}

// Function to fetch URL and populate HttpResponse structure
int UrlFetcher::fetch_url(const std::string& url, HttpResponse& response, bool lab_machine) {
    
    if (curl) {
        response.url = url;

        if(lab_machine){
            const std::string curlCertPath = "./ca-bundle.crt";
            curl_easy_setopt(curl, CURLOPT_CAINFO, curlCertPath.c_str());
        }

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
       

        
        
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response.html_content);
        // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        CURLcode res;

        const int max_retry = 3;
        int attempts = 0;
        do{
            res = curl_easy_perform(curl);
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);
            
            if (res == CURLE_OK && (response.status_code == 200 || response.status_code == 301 || response.status_code == 302 || response.status_code == 404)) {
                break;
            }
        
            logger::warn() << "Attempt " << attempts << " failed. Retry in 5 Seconds. Error: "<< curl_easy_strerror(res) << "; URL: "<< url << logger::endl;
            std::this_thread::sleep_for(std::chrono::seconds(5));

            
        }while (++attempts < max_retry);

        if(res != CURLE_OK){
            logger::error() << "Failed to fetch URL: " << url << logger::endl;
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
        if (response.status_code == 302 || response.status_code == 301) {
            char* redirect_url;
            curl_easy_getinfo(curl, CURLINFO_REDIRECT_URL, &redirect_url);
            if (redirect_url) {
                response.redirect_url = redirect_url;
            }
        }
    } else {
        std::cerr << "[ERROR] Failed to initialize CURL" << std::endl;
        return 1;
    }

    return 0;
}