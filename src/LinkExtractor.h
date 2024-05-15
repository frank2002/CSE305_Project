#ifndef LINKEXTRACTOR_H
#define LINKEXTRACTOR_H

#include <iostream>
#include <string>
#include <vector>
#include <regex>


class LinkExtractor {
public:
    LinkExtractor();
    ~LinkExtractor();
    void extract_links(const std::string& html_content, std::vector<std::string>& links, const std::string& base_url);

private:
    std::regex link_regex;
};

#endif // LINKEXTRACTOR_H