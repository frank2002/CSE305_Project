#include "StripedHashset.h"




template<typename T>
StripedHashset<T>::StripedHashset(int num_stripes) : num_stripes_(num_stripes), mutexes_(num_stripes), buckets_(num_stripes) {}

template<typename T>
StripedHashset<T>::~StripedHashset() {}

template<typename T>
bool StripedHashset<T>::insert(const T& element) {
    if(load_factor() > max_load_factor){
        resize(num_stripes_*2);
    }

    auto& bucket = get_bucket(element);
    auto& mtx = get_mutex(element);

    std::unique_lock lock(mtx);
    bool inserted = bucket.insert(element).second;
    if(inserted){
        size++;
    }
    return inserted;
}

template<typename T>
bool StripedHashset<T>::remove(const T& element) {
    auto& bucket = get_bucket(element);
    auto& mtx = get_mutex(element);

    std::unique_lock lock(mtx);
    return bucket.erase(element) > 0;
}


template<typename T>
bool StripedHashset<T>::contains(const T& element)  {
    auto& bucket = get_bucket(element);
    auto& mtx = get_mutex(element);

    std::shared_lock lock(mtx);
    return bucket.find(element) != bucket.end();
}

template<typename T>
void StripedHashset<T>::resize(size_t new_num_stripes) {
    
    logger::debug() << "Resizing hashset to " << new_num_stripes << " stripes" << logger::endl;
    // std::cout << "Resizing hashset to " << new_num_stripes << " stripes" << std::endl;
    std::vector<std::unique_lock<std::shared_mutex>> locks;
    for (int i = 0; i < num_stripes_; ++i) {
        locks.emplace_back(mutexes_[i]);
    }

    std::vector<std::unordered_set<T>> new_buckets(new_num_stripes);
    for (int i = 0; i < num_stripes_; ++i) {
        for (const auto& element : buckets_[i]) {
            new_buckets[hash_fn_(element) % new_num_stripes].insert(element);
        }
    }

    buckets_ = std::move(new_buckets);
    num_stripes_ = new_num_stripes;
    // mutexes_.resize(new_num_stripes);
    std::vector<std::shared_mutex> new_mutexes(new_num_stripes);
    mutexes_ = std::move(new_mutexes);
}

template<typename T>
std::unordered_set<T>& StripedHashset<T>::get_bucket(const T& element) {
    return buckets_[hash_fn_(element) % num_stripes_];
}

template<typename T>
std::shared_mutex& StripedHashset<T>::get_mutex(const T& element) {
    return mutexes_[hash_fn_(element) % num_stripes_];
}

template<typename T>
float StripedHashset<T>::load_factor() const {
    return static_cast<float>(size) / num_stripes_;
}