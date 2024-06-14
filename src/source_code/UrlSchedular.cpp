#include "UrlScheduler.h"

UrlScheduler::UrlScheduler() {}

UrlScheduler::~UrlScheduler() {}

void UrlScheduler::enqueueUrl(const std::string &url)
{
    std::lock_guard<std::mutex> lock(queueMutex);
    urlQueue.push(url);
    queueCondition.notify_one();
}

std::string UrlScheduler::dequeueUrl()
{
    std::unique_lock<std::mutex> lock(queueMutex);
    if (urlQueue.empty())
    {
        return "";
    }
    std::string url = urlQueue.front();
    urlQueue.pop();
    return url;
} // Very simple FIFO queue implementation with thread-safe mutator methods

bool UrlScheduler::isEmpty()
{
    std::lock_guard<std::mutex> lock(queueMutex);
    return urlQueue.empty();
} // Check if the queue is empty

void UrlScheduler::enqueueUrl_non_blocking(const std::string &url)
{
    urlQueue.push(url);
} // Enqueue a URL without locking the queue

std::string UrlScheduler::dequeueUrl_non_blocking()
{
    if (urlQueue.empty())
    {
        return "";
    }
    std::string url = urlQueue.front();
    urlQueue.pop();
    return url;
} // Dequeue a URL without locking the queue

bool UrlScheduler::isEmpty_non_blocking()
{
    return urlQueue.empty();
} // Check if the queue is empty without locking the queue

void UrlScheduler::notify_one_worker()
{
    queueCondition.notify_one();
} // Notify one worker thread that there is a new URL to fetch

void UrlScheduler::notify_all_workers()
{
    queueCondition.notify_all();
} // Notify all worker threads that there is a new URL to fetch