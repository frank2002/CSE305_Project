#ifndef CRAWLER_H
#define CRAWLER_H

#include <thread>
#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include "UrlStore.h"
#include "UrlFetcher.h"
#include "LinkExtractor.h"
#include "HttpResponse.h"
#include "UrlScheduler.h"   

class Crawler {
public:
    Crawler(const std::string& start_url, const size_t num_threads, const std::string& file_path);
    ~Crawler();

    void start();
    void force_stop();
    void log(const std::string& message);


private:
    std::vector<std::thread> workers;
    std::vector<std::mutex> worker_locks;
    std::condition_variable stop_condition;

    std::mutex termination_mutex;
    std::condition_variable termination_condition;

    std::atomic<int> active_threads = 0;
    std::atomic<bool> running = true;
    size_t num_threads;

    UrlStore url_store = UrlStore();
    UrlScheduler url_scheduler = UrlScheduler();

    std::string start_url;
    std::string base_url;
    std::string domain; 

    std::string file_path;


    void worker_thread();
    int verbose = 1;

};

#endif // CRAWLER_H