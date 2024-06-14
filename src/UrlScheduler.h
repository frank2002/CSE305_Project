#ifndef URLSCHEDULER_H
#define URLSCHEDULER_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <chrono>

class UrlScheduler {
public:
    UrlScheduler();
    ~UrlScheduler();

    void enqueueUrl(const std::string& url);
    std::string dequeueUrl();
    bool isEmpty();

    void enqueueUrl_non_blocking(const std::string& url);
    std::string dequeueUrl_non_blocking();
    bool isEmpty_non_blocking();

    void notify_one_worker();
    void notify_all_workers();


    std::condition_variable queueCondition;
    std::mutex queueMutex;
    //storing starting time of the crawler
    std::chrono::system_clock::time_point start_time;


private:
    std::queue<std::string> urlQueue;

    
};

#endif // URLSCHEDULER_H