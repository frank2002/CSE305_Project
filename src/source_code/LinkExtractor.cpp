#include "LinkExtractor.h"

#include <re2/re2.h> // Include the RE2 library. Just for testing.

LinkExtractor::LinkExtractor() {
    // link_regex = std::regex("<a href=\"(.*?)\"");
}


const std::regex LinkExtractor::link_regex(R"(<a\s+[^>]*href\s*=\s*\"([^\s\"<>]+)\"[^>]*>)");




LinkExtractor::~LinkExtractor() {
}

void LinkExtractor::extract_links(const std::string& html_content, std::unordered_set<std::string>& links, const std::string& base_url) {
    re2::StringPiece input(html_content);
    re2::RE2 link_regex(R"(<a\s+[^>]*href\s*=\s*\"([^\s\"<>]+)\"[^>]*>)");
    re2::StringPiece match;
    std::string link;
    
    while (RE2::FindAndConsume(&input, link_regex, &match)) {
        link = std::string(match);
        if (link.find("javascript:") == 0) {
            continue;
        }
        if (link.find("http") != 0) {
            if (base_url.back() == '/' && link.front() == '/') {
                link = base_url + link.substr(1); // Avoid double slashes
            } else if (base_url.back() != '/' && link.front() != '/') {
                link = base_url + "/" + link;
            } else {
                link = base_url + link;
            }
        }
        links.insert(link);
    }
}

// void LinkExtractor::extract_links(const std::string& html_content, std::unordered_set<std::string>& links, const std::string& base_url) {
//     std::smatch match;
//     std::string::const_iterator search_start(html_content.cbegin());
//     while (std::regex_search(search_start, html_content.cend(), match, link_regex)) {
//         std::string link = match[1].str();
//         if(link.find("javascript:") == 0){
//             search_start = match.suffix().first;
//             continue;
//         }
//         std::size_t pos = link.find_first_of("?#");
//         if (pos != std::string::npos) {
//             link = link.substr(0, pos);
//         }
//         if (link.find("http") != 0) {
//             if (base_url.back() == '/' && link.front() == '/') {
//                 link = base_url + link.substr(1); // Avoid double slashes
//             } else if (base_url.back() != '/' && link.front() != '/') {
//                 link = base_url + "/" + link;
//             } else {
//                 link = base_url + link;
//             }
//         }
//         links.insert(link);
//         search_start = match.suffix().first;
//     }
//     return;
// }

std::vector<std::string> LinkExtractor::filter_links(std::unordered_set<std::string>& links, const std::string& domain, const int strictness) {
    std::vector<std::string> filtered_links;
    std::string search_part;
    if (strictness == 1) {
        search_part = domain;
    } else if (strictness == 0) {
        std::regex domainRegex(R"((?:.*\.)?([a-zA-Z0-9-]+\.[a-zA-Z]{2,}))");
        std::smatch match;
        if (std::regex_search(domain, match, domainRegex) && match.size() > 1) {
            search_part = match[1].str();
        }
        search_part = domain; // return the original domain if the regex fails
    } else {
        throw std::invalid_argument("Invalid strictness level");
    }
    for (const std::string& link : links) {
        if (link.find(search_part) != std::string::npos) {
            filtered_links.push_back(link);
        }
    }
    return filtered_links;
}

