#ifndef PACKAGE_TEST_H
#define PACKAGE_TEST_H

#include <iostream>
#include <thread>
#include <vector>
#include <curl/curl.h>

void fetch_url(const std::string& url);

int test();

#endif // PACKAGE_TEST_H