#ifndef INVENTORY_GET_QUOTE_H
#define INVENTORY_GET_QUOTE_H

#include <string> // yes, that is a lot of headers
#include <vector>
#include <memory>
#include <cctype>
#include <limits>
#include <sstream>
#include <cstdlib>
#include <iostream>
#include "Result.h"

std::unique_ptr<Result> request(const std::string& type="stock");
std::unique_ptr<Result> request(std::string symbol, const std::string& type);
std::vector<std::string> split(std::string information, const std::string& separator, const std::string& ignore="NO");
std::string trim(std::string information, const std::string& target);
std::string get_string(const std::string& prompt);
std::string get_string_lower(const std::string& prompt);
std::string get_string_upper(const std::string& prompt);
double get_double(const std::string& prompt);
std::string dtos(double val);

#endif //INVENTORY_GET_QUOTE_H
