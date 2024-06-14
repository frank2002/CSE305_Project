#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

// Include any necessary headers here
#include <string>

struct HttpResponse
{
    std::string url;
    std::string html_content;
    std::string response;
    int status_code;
    std::string content_type;
    std::string charset;
    std::string base_url;
    std::string domain;
    std::string redirect_url;
};

#endif // HTTPRESPONSE_H