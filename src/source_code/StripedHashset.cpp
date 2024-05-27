#include "StripedHashset.h"




template<typename T>
StripedHashset<T>::StripedHashset(int num_stripes) : num_stripes_(num_stripes) {
    buckets_.resize(num_stripes_);
    for (size_t i = 0; i < num_stripes_; ++i) {
        mutexes_.emplace_back(std::make_unique<std::shared_mutex>());
    }
}

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
    if (new_num_stripes <= num_stripes_) {
        return;
    }

    //start time
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    logger::debug() << "Resizing hashset to " << new_num_stripes << " stripes" << logger::endl;

    // Lock all existing mutexes
    std::vector<std::unique_lock<std::shared_mutex>> locks;
    logger::debug() << "Locking all mutexes" << logger::endl;
    for (size_t i = 0; i < num_stripes_; ++i) {
        locks.emplace_back(*mutexes_[i]);
    }
    logger::debug() << "All mutexes locked" << logger::endl;

    // Create new buckets and rehash elements
    std::vector<std::unordered_set<T>> new_buckets(new_num_stripes);
    logger::debug() << "Rehashing elements" << logger::endl;
    for (size_t i = 0; i < num_stripes_; ++i) {
        for (const auto& element : buckets_[i]) {
            new_buckets[hash_fn_(element) % new_num_stripes].insert(element);
        }
    }
    logger::debug() << "Elements rehashed" << logger::endl;

    buckets_ = std::move(new_buckets);

    std::vector<std::unique_ptr<std::shared_mutex>> new_mutexes;
    new_mutexes.reserve(new_num_stripes);
    for (size_t i = 0; i < new_num_stripes; ++i) {
        if (i < num_stripes_) {
            new_mutexes.emplace_back(std::move(mutexes_[i]));
        } else {
            new_mutexes.emplace_back(std::make_unique<std::shared_mutex>()); // Construct a new shared_mutex
        }
    }
    mutexes_ = std::move(new_mutexes);

    num_stripes_ = new_num_stripes;

    //end time
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    logger::debug() << "Resizing took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms" << logger::endl;

    logger::debug() << "All mutexes unlocked" << logger::endl;
}

template<typename T>
std::unordered_set<T>& StripedHashset<T>::get_bucket(const T& element) {
    return buckets_[hash_fn_(element) % num_stripes_];
}

template<typename T>
std::shared_mutex& StripedHashset<T>::get_mutex(const T& element) {
    return *mutexes_[hash_fn_(element) % num_stripes_];
}

template<typename T>
float StripedHashset<T>::load_factor() const {
    return static_cast<float>(size) / num_stripes_;
}

template<typename T>
size_t StripedHashset<T>::get_size() const {
    return size;
}