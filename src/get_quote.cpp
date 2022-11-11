#include <fmt/core.h>
#include <curl/curl.h>
#include "../include/Result.h"
#include "../include/get_quote.h"

std::string readBuffer; // write_callback() receives data in chunks, so this stores the entire result
std::size_t readBufferBytes = 0;

size_t write_callback(char *information, size_t size, size_t members, void *userdata) {
    size_t total_bytes = size * members;
    readBufferBytes += total_bytes;

    readBuffer.append(information, total_bytes); // store each chunk of information return by curl

    return total_bytes;
}

std::unique_ptr<Result> request(const std::string& type) {
    if (std::getenv("API_KEY") == nullptr) { // API_KEY is needed for iex cloud
        std::cout << "Must set API_KEY before proceeding" << std::endl;
        exit(1);
    }

    std::string ticker = get_string("Enter symbol: "); // ticker is basically the same as symbol
    std::string url = fmt::format("https://cloud.iexapis.com/stable/{}/{}/quote?token={}", type, ticker, std::getenv("API_KEY"));

    CURLcode code;
    CURLcode res;
    CURL* curl = curl_easy_init();
    readBufferBytes = 0; // reset buffer because it is global
    readBuffer.clear();

    if (curl == nullptr) {
        std::cout << "NULL ptr" << std::endl;
        return request(type); // im too lazy to deal with this
    }

    code = curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); // c_str() is used as curl.h is written in c
    code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback); // give curl a function to give information
    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        std::cout << "Error when performing request" << std::endl;
        return request(type);
    }

    while (readBuffer == "Unknown symbol") { // API returns 'Unknown symbol' for invalid symbols
        std::cout << fmt::format("{}: {}", readBuffer, ticker) << std::endl;

        readBufferBytes = 0;
        readBuffer.clear();

        ticker = get_string("Enter symbol: ");
        url = fmt::format("https://cloud.iexapis.com/stable/stock/{}/quote?token={}", ticker, std::getenv("API_KEY"));

        code = curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); // new request with a new (and hopefully valid) symbol
        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cout << "Error when performing request" << std::endl;
        }
    }

    curl_easy_cleanup(curl);

    std::vector<std::string> unwanted = {"{", "}"}; // remove braces from JSON-like result
    for (const std::string& s : unwanted) {
        readBuffer = trim(readBuffer, s);
    }

    std::unique_ptr<Result> results(new Result(split(readBuffer, ",", " ")));

    return results;
}

std::unique_ptr<Result> request(std::string symbol, const std::string& type) { // overloaded method that also takes the symbol if already obtained
    if (std::getenv("API_KEY") == nullptr) {
        std::cout << "Must set API_KEY before proceeding" << std::endl;
        exit(1);
    }

    std::string url = fmt::format("https://cloud.iexapis.com/stable/{}/{}/quote?token={}", type, symbol, std::getenv("API_KEY"));

    CURLcode code;
    CURLcode res;
    CURL* curl = curl_easy_init();
    readBufferBytes = 0;
    readBuffer.clear();

    if (curl == nullptr) {
        std::cout << "NULL ptr" << std::endl;
        return request(symbol, type); // im too lazy to deal with this
    }

    code = curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        std::cout << "Error when performing request" << std::endl;
        return request(symbol, type);
    }

    while (readBuffer == "Unknown symbol") {
        std::cout << fmt::format("{}: {}", readBuffer, symbol) << std::endl;

        readBufferBytes = 0;
        readBuffer.clear();

        symbol = get_string("Enter symbol: ");
        url = fmt::format("https://cloud.iexapis.com/stable/stock/{}/quote?token={}", symbol, std::getenv("API_KEY"));

        code = curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cout << "Error when performing request" << std::endl;
        }
    }

    curl_easy_cleanup(curl);

    std::vector<std::string> unwanted = {"{", "}"};
    for (const std::string& s : unwanted) {
        readBuffer = trim(readBuffer, s);
    }

    std::unique_ptr<Result> results(new Result(split(readBuffer, ",", " ")));

    return results;
}

std::vector<std::string> split(std::string information, const std::string& separator, const std::string& ignore) { // trying to emulate pythons .split() method
    std::vector<std::string> data;

    long last_separator_index = -1;

    for (long unsigned int i = 0; i < information.size(); i++) {
        if (separator == std::string(1, information[i]) && ignore != std::string(1, information.at(i + 1))) { // look for the separator
            data.push_back(information.substr(last_separator_index + 1, i - last_separator_index - 1));
            last_separator_index = i;
        }
    }

    if (last_separator_index != information.size() - 1) { // append remaining information if original string didn't end with the separator
        data.push_back(information.substr(last_separator_index + 1, information.size() - last_separator_index - 1));
    }

    return data;
}

std::string trim(std::string information, const std::string& target) {
    size_t i;
    
    while ((i = information.find(target)) != std::string::npos) { // removes each unwanted substring until none are found
        information.erase(i, target.size());
    }

    return information;
}

std::string get_string(const std::string& prompt) { // mostly for convenience
    std::string str;

    std::cout << prompt;
    getline(std::cin, str);

    while (std::cin.fail()) {
        std::cout << prompt;
        getline(std::cin, str);
    }

    return str;
}

std::string get_string_lower(const std::string& prompt) {
    std::string temp = get_string(prompt);
    std::transform(temp.begin(), temp.end(), temp.begin(), ::tolower); // best I could find without using the boost library

    return temp;
}

std::string get_string_upper(const std::string& prompt) {
    std::string temp = get_string(prompt);
    std::transform(temp.begin(), temp.end(), temp.begin(), ::toupper); // same as above

    return temp;
}

double get_double(const std::string& prompt) {
    double out;

    std::cout << prompt;
    std::cin >> out;

    std::cin.clear();
    std::cin.ignore(std::numeric_limits<int>::max(), '\n'); // just in case

    while (std::cin.fail()) {
        std::cin >> out;

        std::cin.clear();
        std::cin.ignore(std::numeric_limits<int>::max(), '\n');
    }

    return out;
}

std::string dtos(double val) { // double to std::string
    std::ostringstream temp;
    temp << val;
    return temp.str();
}
