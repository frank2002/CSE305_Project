#ifndef LOCK_FREE_SET_H
#define LOCK_FREE_SET_H

#include <atomic>
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <iostream>
#include <fstream>
#include <mutex>

class LockFreeNode {
public:
    std::string value;
    std::atomic<LockFreeNode*> next;

    LockFreeNode(const std::string& val) : value(val), next(nullptr) {}
};

class LockFreeLinkedList {
public:
    LockFreeLinkedList();
    ~LockFreeLinkedList();

    LockFreeLinkedList(LockFreeLinkedList&& other) noexcept;
    LockFreeLinkedList& operator=(LockFreeLinkedList&& other) noexcept;

    bool insert(const std::string& value);
    bool contains(const std::string& value) const;
    bool remove(const std::string& value);

    int get_size() const;

    void save_to_file(std::ofstream& outfile) const;
    

private:
    std::atomic<LockFreeNode*> head;

    LockFreeNode* find(const std::string& value, LockFreeNode** prev) const;
    
};



class LockFreeHashSet {
public:
    LockFreeHashSet(size_t num_buckets);
    ~LockFreeHashSet();

    bool insert(const std::string& value);
    bool contains(const std::string& value) const;
    bool remove(const std::string& value);


    int get_size() const;

    void save_to_file(const std::string& filename) const;

private:
    std::vector<LockFreeLinkedList> buckets;
    std::hash<std::string> hash_fn;
    size_t num_buckets;

    size_t get_bucket_index(const std::string& value) const;


};


#endif // LOCK_FREE_SET_H