#ifndef STRIPEDHASHSET_H
#define STRIPEDHASHSET_H
#define CPPLOG_NAMESPACE logger


#include <vector>
#include <shared_mutex>
#include <mutex>
#include <unordered_set>
#include <thread>
#include <functional>
#include <algorithm>
#include <iostream>
#include <list>

#include <cpplog/log.h>


template<typename T>
class StripedHashset {
public:
    // Constructor
    StripedHashset(int num_stripes);

    // Destructor
    ~StripedHashset();

    // Insert an element into the hashset
    bool insert(const T& element);

    // Remove an element from the hashset
    bool remove(const T& element);

    // Check if an element is present in the hashset
    bool contains(const T& element) ;

    // Resize the bucket count
    void resize(size_t num_stripes);

    // Get the size of the whole hashset
    size_t get_size() const;

private:
    // Number of stripes
    int num_stripes_;
    // Vector of mutexes for each stripe
    // std::vector<std::shared_mutex> mutexes_;
    std::vector<std::unique_ptr<std::shared_mutex>> mutexes_;
    // Vector of hashsets for each stripe
    std::vector<std::unordered_set<T>> buckets_;
    // Hash function
    std::hash<T> hash_fn_;
    
    float max_load_factor = 10;
    size_t size=0;



    std::unordered_set<T>& get_bucket(const T& element) ;

    std::shared_mutex& get_mutex(const T& element) ;

    float load_factor() const;
};

template class StripedHashset<std::string>;


#endif // STRIPEDHASHSET_H