#include "Crawler.h"
#include <cpplog/log.h>

#define CPPLOG_NAMESPACE logger

Crawler::Crawler(const std::string &start_url, const size_t num_threads, const std::string &file_path, size_t max_urls, std::chrono::seconds max_time, bool lab_machine) : num_threads(num_threads),
                                                                                                                                                                           active_threads(0),
                                                                                                                                                                           start_url(start_url),
                                                                                                                                                                           file_path(file_path),
                                                                                                                                                                           max_urls(max_urls),
                                                                                                                                                                           max_time(max_time),
                                                                                                                                                                           lab_machine(lab_machine)
{
    curl_global_init(CURL_GLOBAL_ALL);
}

Crawler::~Crawler()
{
    curl_global_cleanup();
}

void Crawler::start()
{
    // std::cout<<"Starting Crawler"<<std::endl;
    logger::info() << "Starting Crawler" << logger::endl;

    url_store.set_filename(file_path);
    url_scheduler.enqueueUrl(start_url);

    int ending_signal = -1;

    for (size_t i = 0; i < num_threads; ++i)
    {
        workers.push_back(std::thread(&Crawler::worker_thread, this));
    }
    auto start_time = std::chrono::system_clock::now();
    url_scheduler.start_time = start_time;

    logger::debug() << "max_time: " << max_time.count() << " seconds" << logger::endl;
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::unique_lock<std::mutex> lock(termination_mutex);

        termination_condition.wait(lock, [this, start_time]
                                   {
            auto elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start_time);
            logger::debug() << "Elapsed time: " << elapsed_time.count() << " seconds" << logger::endl;
            return (
                (url_scheduler.isEmpty() && active_threads == 0)    // all threads are done
                || (max_urls > 0 && url_store.get_visited_urls_size() >= max_urls)    // max number of URLs reached
                || (max_time.count() > 0 && max_time.count() <= elapsed_time.count())     // max time reached
            ); });

        auto elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start_time);
        logger::debug() << "Elapsed time: " << elapsed_time.count() << " seconds" << logger::endl;

        if (max_urls > 0 && url_store.get_visited_urls_size() >= max_urls)
        {
            logger::info() << "\033[31mReached the maximum limit of visited URLs: " << max_urls << "\033[0m" << logger::endl;
            running = false;
            url_scheduler.notify_all_workers();
            ending_signal = 2;
            break;
        }

        if (max_time.count() > 0 && max_time.count() <= elapsed_time.count())
        {
            logger::info() << "\033[31mReached the maximum time limit: " << max_time.count() << " seconds\033[0m" << logger::endl;
            running = false;
            url_scheduler.notify_all_workers();
            ending_signal = 1;
            break;
        }

        if (url_scheduler.isEmpty() && active_threads == 0)
        {
            running = false;
            url_scheduler.notify_all_workers();
            ending_signal = 0;
            break;
        }
    }

    for (auto &worker : workers)
    {
        if (worker.joinable())
        {
            worker.join();
        }
    }

    logger::info() << "\033[32mSaving URLs to file. Please wait!\033[0m" << logger::endl;
    url_store.save_found_url_to_file(file_path);
    logger::info() << "\033[32mURLs saved to file\033[0m" << logger::endl
                   << "=============================================" << logger::endl;

    if (ending_signal == -1)
    {
        logger::error() << "\033[31mCrawler ended with unknown error\033[0m" << logger::endl;
    }
    else if (ending_signal == 0)
    {
        logger::info() << "\033[32mCrawler ended successfully\033[0m" << logger::endl;
    }
    else if (ending_signal == 1)
    {
        logger::info() << "\033[32mCrawler ended due to time limit\033[0m" << logger::endl;
    }
    else if (ending_signal == 2)
    {
        logger::info() << "\033[32mCrawler ended due to URL limit\033[0m" << logger::endl;
    }

    num_visited_urls = url_store.get_visited_urls_size();
    num_found_urls = url_store.get_found_urls_size();

    logger::info() << "\033[32mVisited URLs: " << url_store.get_visited_urls_size() << "\033[0m" << logger::endl;
}

void Crawler::log(const std::string &message)
{
    std::cout << message << std::endl;
}

void Crawler::worker_thread()
{
    while (running)
    {

        std::string current_url;
        {
            std::unique_lock<std::mutex> lock(url_scheduler.queueMutex);
            // std::cout << url_scheduler.isEmpty_non_blocking() << std::endl;
            // std::cout << "num_active_threads: " << active_threads << std::endl;
            url_scheduler.queueCondition.wait(lock, [this]
                                              { return !url_scheduler.isEmpty_non_blocking() || !running; });
            if (!running && url_scheduler.isEmpty_non_blocking())
            {
                // std::cout << "Thread " << std::this_thread::get_id() << " is terminating" << std::endl;
                logger::debug() << "Thread " << std::this_thread::get_id() << " is terminating" << logger::endl;
                return;
            }
            if (!url_scheduler.isEmpty_non_blocking())
            {
                current_url = url_scheduler.dequeueUrl_non_blocking();
                if (url_store.search_visited_url(current_url))
                {
                    // logger::debug() << " skipping visited URL: " << current_url << logger::endl;
                    continue;
                }
                active_threads++;
            }
            else
            {
                continue;
            }
        }

        HttpResponse response;
        UrlFetcher fetcher;

        fetcher.fetch_url(current_url, response, lab_machine);

        // std::cout << "[Success] Thread " << std::this_thread::get_id() << " fetched: (" << response.status_code <<") " <<current_url << std::endl;
        std::string colored_status_code;
        if (response.status_code == 200)
        {
            // green
            colored_status_code = "\033[1;32m" + std::to_string(response.status_code) + "\033[0m";
        }
        else
        {
            // yellow
            colored_status_code = "\033[1;33m" + std::to_string(response.status_code) + "\033[0m";
        }
        logger::info() << "Fetched: (" << colored_status_code << ") " << current_url << logger::endl;

        if (response.status_code == 200)
        {
            // check if the url is HTML content
            if (response.content_type.find("text/html") != std::string::npos)
            {
                // url_store.write_url(response.url);
                url_store.add_found_url(response.url);
                url_store.add_visited_url(response.url);
                LinkExtractor extractor;
                std::unordered_set<std::string> links;
                extractor.extract_links(response.html_content, links, response.base_url);
                std::vector<std::string> filtered_links;
                filtered_links = extractor.filter_links(links, response.domain, 1); // make sure the links are from the same domain
                std::vector<std::string> new_links = url_store.filter_out_visited_urls(filtered_links);

                for (const auto &link : new_links)
                {
                    // url_store.write_url(link, response.url);
                    url_store.add_found_url(link);
                    if (link.find("https://en.wikipedia.org/#cite") == 0)
                    {
                        continue;
                    }
                    url_scheduler.enqueueUrl(link);
                }
            }
            else
            {
                url_store.add_visited_url(response.url);
            }
        }
        else if (response.status_code == 302 || response.status_code == 301)
        {
            url_store.add_visited_url(response.url);
            url_scheduler.enqueueUrl(response.redirect_url);
            // url_store.write_url(response.redirect_url, response.url);
            url_store.add_found_url(response.redirect_url);
            // logger::info() << "Redirecting to: " << response.redirect_url << logger::endl;
        }
        else
        {
            url_store.add_visited_url(response.url);
        }

        {
            std::lock_guard<std::mutex> lock(termination_mutex);
            active_threads--;
            if (url_scheduler.isEmpty() && active_threads == 0 || (max_urls > 0 && url_store.get_visited_urls_size() >= max_urls) || (max_time.count() > 0 && std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - url_scheduler.start_time).count() >= max_time.count()))
            {
                termination_condition.notify_one();
            }
        }
    }
}