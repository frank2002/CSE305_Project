#ifndef URLSTORE_H
#define URLSTORE_H

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <mutex>
#include <unordered_set>

#include "StripedHashset.h"

class UrlStore {
public:
    UrlStore();
    ~UrlStore();

    void set_filename(const std::string& filename);
    void write_url(const std::string& visited_url, const std::string& from_url);
    void write_url(const std::string& visited_url);

    void add_visited_url(const std::string& url); //simple unordered set for baseline solution
    bool search_visited_url(const std::string& url);

    std::vector<std::string> filter_out_visited_urls(std::vector<std::string>& urls);

    void add_visited_url_simple(const std::string& url);
    void add_visited_url_striped(const std::string& url);
    bool search_visited_url_striped(const std::string& url);
    bool search_visited_url_simple(const std::string& url);

    void add_found_url(const std::string& url);

    size_t get_visited_urls_size() const;
    size_t get_found_urls_size() const;

    void save_found_url_to_file(const std::string& filename);


private:
    std::string filename;
    std::ofstream file_stream;

    std::mutex file_lock; //lock to protect file access

    std::unordered_set<std::string> visited_urls; //store visited URLs to avoid duplicates (Simple solution)
    StripedHashset<std::string> striped_visited_urls; //store visited URLs to avoid duplicates (StripedHashset solution)

    std::unordered_set<std::string> found_urls; //store found URLs to avoid duplicates (Simple solution)
    StripedHashset<std::string> striped_found_urls; //store found URLs to avoid duplicates (StripedHashset solution)

    
    std::mutex visited_urls_lock; //lock to protect visited_urls access
    

    int data_structure = 1; //0 for simple unordered set, 1 for StripedHashset


};

#endif // URLSTORE_H