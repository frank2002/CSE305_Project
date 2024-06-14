#include "UrlStore.h"

UrlStore::UrlStore(int cache_type): striped_visited_urls(64, 1), striped_found_urls(64, 2), data_structure(cache_type), lock_free_found_urls(64), lock_free_visited_urls(64){
    
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
    // logger::debug() << "Data structure: " << data_structure << logger::endl;
    auto start = std::chrono::high_resolution_clock::now();
    if (data_structure == 0) {
        add_visited_url_simple(url);
    } else if (data_structure == 1) {
        add_visited_url_striped(url);
    } else {
        add_visited_url_lock_free(url);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end-start);
    insertion_time += duration;
    
}


void UrlStore::add_visited_url_batch(const std::vector<std::string>& urls) {
    auto start = std::chrono::high_resolution_clock::now();
    if (data_structure == 0) {
        for (const auto& url : urls) {
            add_visited_url_simple(url);
        }
    } else if (data_structure==1){
        add_visited_url_striped_batch(urls);
    } else {
        for (const auto& url : urls) {
            add_visited_url_lock_free(url);
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end-start);
    insertion_time += duration;
}

void UrlStore::add_found_url_batch(const std::vector<std::string>& urls) {
    auto start = std::chrono::high_resolution_clock::now();
    if (data_structure == 0) {
        for (const auto& url : urls) {
            found_urls.insert(url);
        }
    } else if (data_structure == 1){
        striped_found_urls.insert_batch(urls);
    } else {
        // logger::debug() << "Adding found urls to lock free set" << logger::endl;
        for (const auto& url : urls) {
            lock_free_found_urls.insert(url);
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end-start);
    insertion_time += duration;
}

void UrlStore::add_visited_url_simple(const std::string& url) {
    std::lock_guard<std::mutex> lock(visited_urls_lock);
    visited_urls.insert(url);
}

void UrlStore::add_visited_url_striped_batch(const std::vector<std::string>& urls) {
    striped_visited_urls.insert_batch(urls);
}

void UrlStore::add_visited_url_striped(const std::string& url) {
    striped_visited_urls.insert(url);
}

void UrlStore::add_visited_url_lock_free(const std::string& url) {
    lock_free_visited_urls.insert(url);
}




bool UrlStore::search_visited_url(const std::string& url) {
    auto start = std::chrono::high_resolution_clock::now();
    if (data_structure == 0) {
        return search_visited_url_simple(url);
    } else if (data_structure == 1) {
        return search_visited_url_striped(url);
    } else {
        return search_visited_url_lock_free(url);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end-start);
    search_time += duration;
}

bool UrlStore::search_visited_url_simple(const std::string& url) {
    std::lock_guard<std::mutex> lock(visited_urls_lock);
    return visited_urls.find(url) != visited_urls.end();
}

bool UrlStore::search_visited_url_striped(const std::string& url) {
    return striped_visited_urls.contains(url);
}

bool UrlStore::search_visited_url_lock_free(const std::string& url) {
    return lock_free_visited_urls.contains(url);
}

std::vector<std::string> UrlStore::filter_out_visited_urls_simple(std::vector<std::string>& urls) {
    std::vector<std::string> filtered_urls;
    std::lock_guard<std::mutex> lock(visited_urls_lock);
    for (const auto& url : urls) {
        if (visited_urls.find(url) == visited_urls.end()) {
            filtered_urls.push_back(url);
        }
    }
    return filtered_urls;
}

std::vector<std::string> UrlStore::filter_out_visited_urls_striped(std::vector<std::string>& urls) {
    std::vector<std::string> filtered_urls; 
    for (const auto& url : urls) {
        if (!striped_visited_urls.contains(url)) {
            filtered_urls.push_back(url);
        }
    }
    return filtered_urls;
}

std::vector<std::string> UrlStore::filter_out_visited_urls_lock_free(std::vector<std::string>& urls) {
    std::vector<std::string> filtered_urls;
    for (const auto& url : urls) {
        if (!lock_free_visited_urls.contains(url)) {
            filtered_urls.push_back(url);
        }
    }
    return filtered_urls;
}

std::vector<std::string> UrlStore::filter_out_visited_urls(std::vector<std::string>& urls) {
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<std::string> filtered_urls;
    if (data_structure == 0) {
        filtered_urls = filter_out_visited_urls_simple(urls);
    } else if (data_structure == 1) {
        
        filtered_urls = filter_out_visited_urls_striped(urls);
    } else {
        filtered_urls = filter_out_visited_urls_lock_free(urls);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end-start);
    search_time += duration;
    return filtered_urls;
}


size_t UrlStore::get_visited_urls_size() const {
    if (data_structure == 0) {
        return visited_urls.size();
    } else if (data_structure == 1) {
        return striped_visited_urls.get_size();
    } else {
        return lock_free_visited_urls.get_size();
    }
}

size_t UrlStore::get_found_urls_size() const {
    if (data_structure == 0) {
        return found_urls.size();
    } else if (data_structure == 1) {
        return striped_found_urls.get_size();
    } else {
        return lock_free_found_urls.get_size();
    }
}


void UrlStore::add_found_url(const std::string& url) {
    auto start = std::chrono::high_resolution_clock::now();

    if (data_structure == 0) {
        found_urls.insert(url);
        // logger::debug() << "1" << logger::endl;
    } else if (data_structure == 1) {
        striped_found_urls.insert(url);
        // logger::debug()<< "2" << logger::endl;
    } else {
        lock_free_found_urls.insert(url);
        // logger::debug() << "3" << logger::endl;
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end-start);
    insertion_time += duration;

}


void UrlStore::save_found_url_to_file(const std::string& filename) {
    if (data_structure == 0) {
        std::this_thread::sleep_for(std::chrono::seconds(1)); // just for testing purpose. Not implemented.
    }else if (data_structure == 1){
        striped_found_urls.save_to_file(filename);
    } else {
        lock_free_found_urls.save_to_file(filename);
    }
    // striped_found_urls.save_to_file(filename);
}

void UrlStore::save_found_url_to_file_simple(const std::string& filename) {
    std::ofstream outfile(filename, std::ios::out);
    if (!outfile.is_open()) {
        throw std::runtime_error("Failed to open file");
    }
    for (const auto& url : found_urls) {
        file_stream << url << std::endl;
    }
    file_stream.close();
}


void UrlStore::logging_operation_time() {
    logger::info() << "Insertion time: " << insertion_time.count() / 1E6 << " s" << logger::endl;
    logger::info() << "Search time: " << search_time.count() /1E6<< " s" << logger::endl;
}