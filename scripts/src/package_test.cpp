#include "package_test.h"

// Function to perform HTTP GET request
void fetch_url(const std::string &url)
{
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }
        else
        {
            std::cout << "Successfully fetched: " << url << std::endl;
        }
        curl_easy_cleanup(curl);
    }
    else
    {
        std::cerr << "Failed to initialize CURL" << std::endl;
    }
}

// Test function to test threading and CURL
int test()
{
    // List of URLs to fetch
    std::vector<std::string> urls = {
        "https://www.example.com",
        "https://www.wikipedia.org",
        "https://www.google.com"};

    std::vector<std::thread> threads;

    CURLcode global_init = curl_global_init(CURL_GLOBAL_DEFAULT);
    if (global_init != CURLE_OK)
    {
        std::cerr << "curl_global_init() failed: " << curl_easy_strerror(global_init) << std::endl;
        return 1;
    }

    for (const auto &url : urls)
    {
        threads.emplace_back(fetch_url, url);
    }

    for (auto &thread : threads)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }

    curl_global_cleanup();

    std::cout << "All threads completed" << std::endl;
    return 0;
}