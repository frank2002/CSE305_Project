#include "UrlStore.h"

UrlStore::UrlStore(): striped_visited_urls(512, 1), striped_found_urls(4096, 2){
    
}

UrlStore::~UrlStore() {
    file_stream.close();
}

void UrlStore::set_filename(const std::string& filename) {
    this->filename = filename;
    file_stream.open(filename, std::ios::out);
}

void UrlStore::write_url(const std::string& visited_url, const std::string& from_url) {
    std::lock_guard<std::mutex> lock(file_lock);
    file_stream << visited_url << "\t" << from_url << std::endl;
}

void UrlStore::write_url(const std::string& visited_url) {
    std::lock_guard<std::mutex> lock(file_lock);
    file_stream << visited_url << std::endl;
}


/// @brief Add a visited URL to the cache store
/// @param url 
void UrlStore::add_visited_url(const std::string& url) {
    if (data_structure == 0) {
        add_visited_url_simple(url);
    } else {
        add_visited_url_striped(url);
    }
    
}

void UrlStore::add_visited_url_simple(const std::string& url) {
    std::lock_guard<std::mutex> lock(visited_urls_lock);
    visited_urls.insert(url);
}

void UrlStore::add_visited_url_striped(const std::string& url) {
    striped_visited_urls.insert(url);
}


bool UrlStore::search_visited_url(const std::string& url) {
    if (data_structure == 0) {
        return search_visited_url_simple(url);
    } else {
        return search_visited_url_striped(url);
    }
}

bool UrlStore::search_visited_url_simple(const std::string& url) {
    std::lock_guard<std::mutex> lock(visited_urls_lock);
    return visited_urls.find(url) != visited_urls.end();
}

bool UrlStore::search_visited_url_striped(const std::string& url) {
    return striped_visited_urls.contains(url);
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


size_t UrlStore::get_visited_urls_size() const {
    if (data_structure == 0) {
        return visited_urls.size();
    } else {
        return striped_visited_urls.get_size();
    }
}


void UrlStore::add_found_url(const std::string& url) {
    striped_found_urls.insert(url);
}


void UrlStore::save_found_url_to_file(const std::string& filename) {
    striped_found_urls.save_to_file(filename);
}