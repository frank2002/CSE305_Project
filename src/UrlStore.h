#ifndef URLSTORE_H
#define URLSTORE_H

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <mutex>
#include <unordered_set>

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
    
    
    
    
private:
    std::string filename;
    std::ofstream file_stream;

    std::mutex file_lock; //lock to protect file access

    std::unordered_set<std::string> visited_urls; //store visited URLs to avoid duplicates (Simple solution)
    
    std::mutex visited_urls_lock; //lock to protect visited_urls access


};

#endif // URLSTORE_H