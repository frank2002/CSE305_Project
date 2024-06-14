#include "LockFreeSet.h"

LockFreeLinkedList::LockFreeLinkedList() : head(nullptr) {}

LockFreeLinkedList::~LockFreeLinkedList() {
    LockFreeNode* node = head.load();
    while (node) {
        LockFreeNode* next = node->next.load();
        delete node;
        node = next;
    }
}

// Move constructor
LockFreeLinkedList::LockFreeLinkedList(LockFreeLinkedList&& other) noexcept : head(other.head.exchange(nullptr)) {}

// Move assignment operator
LockFreeLinkedList& LockFreeLinkedList::operator=(LockFreeLinkedList&& other) noexcept {
    if (this != &other) {
        // Free the existing resources
        LockFreeNode* node = head.exchange(nullptr);
        while (node) {
            LockFreeNode* next = node->next.load();
            delete node;
            node = next;
        }
        // Move the head pointer from the other list
        head = other.head.exchange(nullptr);
    }
    return *this;
}

bool LockFreeLinkedList::insert(const std::string& value) {
    LockFreeNode* newNode = new LockFreeNode(value);
    while (true) {
        LockFreeNode* node = head.load();
        newNode->next = node;
        if (head.compare_exchange_weak(node, newNode)) {
            return true;
        }
    }
}

bool LockFreeLinkedList::contains(const std::string& value) const {
    LockFreeNode* node = head.load();
    while (node) {
        if (node->value == value) {
            return true;
        }
        node = node->next.load();
    }
    return false;
}

bool LockFreeLinkedList::remove(const std::string& value) {
    while (true) {
        LockFreeNode* prev = nullptr;
        LockFreeNode* node = find(value, &prev);
        if (!node) {
            return false;
        }
        LockFreeNode* next = node->next.load();
        if (node->next.compare_exchange_weak(next, next)) {
            if (prev) {
                prev->next.compare_exchange_weak(node, next);
            } else {
                head.compare_exchange_weak(node, next);
            }
            delete node;
            return true;
        }
    }
}

LockFreeNode* LockFreeLinkedList::find(const std::string& value, LockFreeNode** prev) const {
    LockFreeNode* node = head.load();
    *prev = nullptr;
    while (node) {
        if (node->value == value) {
            return node;
        }
        *prev = node;
        node = node->next.load();
    }
    return nullptr;
}

int LockFreeLinkedList::get_size() const {
    int size = 0;
    LockFreeNode* node = head.load();
    while (node) {
        size++;
        node = node->next.load();
    }
    return size;
}

LockFreeHashSet::LockFreeHashSet(size_t num_buckets) : num_buckets(num_buckets) {
    buckets.resize(num_buckets);
}

LockFreeHashSet::~LockFreeHashSet() {}

bool LockFreeHashSet::insert(const std::string& value) {
    size_t index = get_bucket_index(value);
    return  buckets[index].insert(value);


}

bool LockFreeHashSet::contains(const std::string& value) const {
    size_t index = get_bucket_index(value);
    return buckets[index].contains(value);
}

bool LockFreeHashSet::remove(const std::string& value) {
    size_t index = get_bucket_index(value);
    return buckets[index].remove(value);
}

size_t LockFreeHashSet::get_bucket_index(const std::string& value) const {
    return hash_fn(value) % num_buckets;
}


int LockFreeHashSet::get_size() const {
    int size = 0;
    for (const auto& bucket : buckets) {
        size += bucket.get_size();
    }
    return size;
}

void LockFreeLinkedList::save_to_file(std::ofstream& outfile) const {
    LockFreeNode* node = head.load();
    while (node) {
        outfile << node->value << std::endl;
        node = node->next.load();
    }
}

void LockFreeHashSet::save_to_file(const std::string& filename) const {
    std::ofstream outfile(filename);
    if (!outfile.is_open()) {
        throw std::runtime_error("Failed to open file");
    }

    for (const auto& bucket : buckets) {
        bucket.save_to_file(outfile);
    }

    outfile.close();
}





