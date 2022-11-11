#include "../include/Result.h"
#include "../include/get_quote.h"

Result::Result(const std::vector<std::string>& information) {
    for (const std::string& line : information) { // parse long string into the class' vectors
        std::vector<std::string> temp = split(line, ":");

        key.push_back(trim(temp[0], "\""));
        value.push_back(trim(temp[1], "\""));
    }
}

double Result::get_number(const std::string& field) const {
    std::string temp = get_data(field);
    if (temp == "null") {
        return -1;
    }
    return std::stod(temp); // std::string to double
}

std::string Result::get_data(const std::string& field) const { // treats the vectors as a lookup table, albeit inefficient
    for (size_t i = 0; i < key.size(); i++) {
        if (key[i] == field) {
            return value[i];
        }
    }
    return "";
}
