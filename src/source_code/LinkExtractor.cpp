#include "LinkExtractor.h"

LinkExtractor::LinkExtractor() {
    link_regex = std::regex("<a href=\"(.*?)\"");
}

LinkExtractor::~LinkExtractor() {
}

void LinkExtractor::extract_links(const std::string& html_content, std::vector<std::string>& links, const std::string& base_url) {
    std::smatch match;
    std::string::const_iterator search_start(html_content.cbegin());
    while (std::regex_search(search_start, html_content.cend(), match, link_regex)) {
        std::string link = match[1].str();
        if (link.find("http") == std::string::npos) {
            link = base_url + link;
        }
        links.push_back(link);
        search_start = match.suffix().first;
    }
    return;
}

<<<<<<< HEAD
=======
void LinkExtractor::extract_links(const std::string& html_content, std::unordered_set<std::string>& links, const std::string& base_url) {
    std::smatch match;
    std::string::const_iterator search_start(html_content.cbegin());
    while (std::regex_search(search_start, html_content.cend(), match, link_regex)) {
        std::string link = match[1].str();
        if (link.find("http") == std::string::npos) {
            link = base_url + link;
        }
        links.insert(link);
        search_start = match.suffix().first;
    }
    return;
}

>>>>>>> aad831c (Upload Link extractor & Add HttpResponse struture & modify URLFetcher)
