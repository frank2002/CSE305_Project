#include "Crawler.h"
#include <string>
#include <cpplog/log.h>

#define CPPLOG_NAMESPACE logger

Crawler::Crawler(const std::string& start_url, const size_t num_threads, const std::string& file_path) :
    num_threads(num_threads),
    active_threads(0),
    start_url(start_url),
    file_path(file_path){
    curl_global_init(CURL_GLOBAL_ALL);
}

Crawler::~Crawler() {
    curl_global_cleanup();
}

void Crawler::start() {
    // std::cout<<"Starting Crawler"<<std::endl;
    logger::info() << "Starting Crawler" << logger::endl;

    url_store.set_filename(file_path);
    url_scheduler.enqueueUrl(start_url);


    for (size_t i = 0; i < num_threads; ++i) {
        workers.push_back(std::thread(&Crawler::worker_thread, this));
    }

    while(true){
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::unique_lock<std::mutex> lock(termination_mutex);
        termination_condition.wait(lock, [this] {
            return url_scheduler.isEmpty() && active_threads == 0;
        });

        if (url_scheduler.isEmpty() && active_threads == 0) {
            running = false;
            url_scheduler.notify_all_workers();
            break;
        }

    }

    for (auto& worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }


    
}

void Crawler::log(const std::string& message) {
    std::cout << message << std::endl;
}


void Crawler::worker_thread() {
    while(running){

        std::string current_url;
        {
            std::unique_lock<std::mutex> lock(url_scheduler.queueMutex);
            // std::cout << url_scheduler.isEmpty_non_blocking() << std::endl;
            // std::cout << "num_active_threads: " << active_threads << std::endl;
            url_scheduler.queueCondition.wait(lock, [this] { return !url_scheduler.isEmpty_non_blocking() || !running; });
            if(!running && url_scheduler.isEmpty_non_blocking()){
                // std::cout << "Thread " << std::this_thread::get_id() << " is terminating" << std::endl;
                logger::debug() << "Thread " << std::this_thread::get_id() << " is terminating" << logger::endl;
                return;
            }
            if (!url_scheduler.isEmpty_non_blocking()) {
                current_url = url_scheduler.dequeueUrl_non_blocking();
                active_threads++;
            } else {
                continue;
            }
        }



        HttpResponse response;
        UrlFetcher fetcher;

        fetcher.fetch_url(current_url, response);

        
        // std::cout << "[Success] Thread " << std::this_thread::get_id() << " fetched: (" << response.status_code <<") " <<current_url << std::endl;
        std::string colored_status_code;
        if (response.status_code == 200) {
            //green
            colored_status_code = "\033[1;32m" + std::to_string(response.status_code) + "\033[0m";
        } else {
            //yellow
            colored_status_code = "\033[1;33m" + std::to_string(response.status_code) + "\033[0m";
        }
        logger::info() << "Fetched: (" << colored_status_code <<") " <<current_url << logger::endl;
  

        if(response.status_code == 200){
            // check if the url is HTML content
            if(response.content_type.find("text/html") != std::string::npos){
                url_store.write_url(response.url);
                url_store.add_visited_url(response.url);
                LinkExtractor extractor;
                std::unordered_set<std::string> links;
                extractor.extract_links(response.html_content, links, response.base_url);
                std::vector<std::string> filtered_links;
                filtered_links = extractor.filter_links(links, response.domain, 1);
                std::vector<std::string> new_links = url_store.filter_out_visited_urls(filtered_links);
                
                for(const auto& link : new_links){
                    url_scheduler.enqueueUrl(link);
                    url_store.write_url(link, response.url);
                }

            } else {
                url_store.add_visited_url(response.url);
            }
        }else if(response.status_code == 302 || response.status_code == 301){
            url_store.add_visited_url(response.url);
            url_scheduler.enqueueUrl(response.redirect_url);
            url_store.write_url(response.redirect_url, response.url);
            logger::info() << "Redirecting to: " << response.redirect_url << logger::endl;
        } else {
            url_store.add_visited_url(response.url);
        }

        {
            std::lock_guard<std::mutex> lock(termination_mutex);
            active_threads--;
            if (url_scheduler.isEmpty() && active_threads == 0) {
                termination_condition.notify_one();
            }
        }

        

    }
}