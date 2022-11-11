#ifndef INVENTORY_RESULT_H
#define INVENTORY_RESULT_H

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

class Result {
    public:
        Result(const std::vector<std::string>& information);
        double get_number(const std::string& field) const;
        std::string get_data(const std::string& field) const;
    private:
        std::vector<std::string> key;
        std::vector<std::string> value;
};

#endif //INVENTORY_RESULT_H
