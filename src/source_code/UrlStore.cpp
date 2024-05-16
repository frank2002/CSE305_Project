#include "UrlStore.h"

UrlStore::UrlStore() {
}

UrlStore::~UrlStore() {
    file_stream.close();
}

void UrlStore::set_filename(const std::string& filename) {
    this->filename = filename;
    file_stream.open(filename, std::ios::out | std::ios::app);
}

void UrlStore::write_url(const std::string& visited_url, const std::string& from_url) {
    std::lock_guard<std::mutex> lock(file_lock);
    file_stream << visited_url << "\t" << from_url << std::endl;
}

void UrlStore::write_url(const std::string& visited_url) {
    std::lock_guard<std::mutex> lock(file_lock);
    file_stream << visited_url << std::endl;
}

void UrlStore::add_visited_url(const std::string& url) {
    std::lock_guard<std::mutex> lock(visited_urls_lock);
    visited_urls.insert(url);
}

bool UrlStore::search_visited_url(const std::string& url) {
    std::lock_guard<std::mutex> lock(visited_urls_lock);
    return visited_urls.find(url) != visited_urls.end();
}

std::vector<std::string> UrlStore::filter_out_visited_urls(std::vector<std::string>& urls) {
    std::vector<std::string> filtered_urls;
    std::lock_guard<std::mutex> lock(visited_urls_lock);
    for (const auto& url : urls) {
        if (visited_urls.find(url) == visited_urls.end()) {
            filtered_urls.push_back(url);
        }
    }
    return filtered_urls;
}