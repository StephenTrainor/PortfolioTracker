#include <fmt/core.h>
#include <fmt/color.h>
#include "../include/core.h"
#include "../include/Items.h"
#include "../include/get_quote.h"

bool in(const std::vector<std::string>& arr, const std::string& target);
void quote();
void create();
void open();

void start() { // most methods below are pretty self-explanatory and handle the messy branching
    std::vector<std::string> quit = {"q", "quit", "exit", "stop"};

    std::string choice = get_string_lower("1: Quote\n"
                                    "2: Create portfolio file\n"
                                    "3: Open a portfolio file\n"
                                    "> ");

    while (!in(quit, choice)) {
        if (choice == "1") {
            quote();
        }
        else if (choice == "2") {
            create();
        }
        else if (choice == "3") {
            open();
        }
        else {
            fmt::print("Unknown option, please try again or enter \'q\' to quit\n");
        }

        choice = get_string_lower("1: Quote\n"
                                  "2: Create portfolio file\n"
                                  "3: Open a portfolio file\n"
                                  "> ");
    }
}

void quote() {
    auto fgText = fg(fmt::color::antique_white);

    std::vector<std::string> typesAllowed = {"stock", "crypto"};
    std::string type = get_string_lower("\nEnter asset type: ");

    while (!in(typesAllowed, type)) {
        fmt::print("\'{}\' is not an allowed asset type.\n", type);
        type = get_string_lower("Enter asset type: ");
    }

    std::string symbol = get_string("Enter symbol: ");
    std::unique_ptr<Result> data = request(symbol, type);

    // Must separate print statements to individually color and emphasize certain text
    if (type == "crypto") {
        fmt::print(fgText | fmt::emphasis::underline,
                   "\n{} ({}):\n", symbol, data->get_data("sector"));
        fmt::print(fgText,
                   "${:.2f}\n\n", data->get_number("latestPrice"));
    }
    else {
        fmt::print(fgText | fmt::emphasis::underline,
                   "\n{} ({}):\n", data->get_data("companyName"), data->get_data("symbol"));
        fmt::print(fgText,
                   "${:.2f} ", data->get_number("latestPrice"));

        double percentChange = 100 * data->get_number("changePercent");
        auto color = (percentChange < 0) ? fmt::color::crimson : fmt::color::green;

        fmt::print(fg(color),
                   "{:+.2f} ({:+.2f}%)\n", data->get_number("change"), percentChange);

        std::string volume = data->get_data("latestVolume");
        std::string pe = data->get_data("peRatio");

        if (volume == "null") {
            volume = "N/A";
        }
        if (pe == "null") {
            pe = "N/A";
        }

        fmt::print(fgText,
                   "{:<7} {}\n{:<7} {}\n\n", "Volume:", volume, "PE:", pe);
    }
}

void create() {
    std::string filename = get_string("\nEnter desired name of portfolio file: ");
    std::fstream file(filename, std::ios::binary | std::ios::out);

    fmt::print("Enter the type, symbol, and volume of each asset or nothing to stop\n\n");

    std::vector<std::string> typesAllowed = {"stock", "crypto"};
    std::string type = get_string_lower("Enter type of asset: ");

    while (!in(typesAllowed, type)) {
        fmt::print("\'{}\' is not an allowed asset type.\n", type);
        type = get_string_lower("Enter type of asset: ");
    }

    std::string symbol = get_string_upper("Enter symbol: ");
    double volume = get_double("Enter volume of asset: ");

    while (!symbol.empty() && volume >= 0) {
        std::unique_ptr<Result> data = request(symbol, type);

        if (type == "stock") {
            file << data->get_data("companyName");
        }
        else {
            file << " ";
        }

        file << fmt::format(",{},{},{:.2f},{}\n", type, symbol, data->get_number("latestPrice"), volume);

        type = get_string_lower("\nEnter type of asset: ");

        while (!in(typesAllowed, type)) {
            fmt::print("\'{}\' is not an allowed asset type.\n", type);
            type = get_string_lower("Enter type of asset: ");
        }

        symbol = get_string_upper("Enter symbol: ");
        volume = get_double("Enter volume of asset: ");
    }

    file.close();
}

void open() {
    std::unique_ptr<Items> inventoryObject(new Items());
    std::vector<std::string> quit = {"q", "quit", "exit", "stop"};

    std::string choice = get_string_lower("\n1: Quote\n"
                                          "2: Display portfolio\n"
                                          "3: Remove units/shares\n"
                                          "4: Add units/shares\n"
                                          "> ");

    while (!in(quit, choice)) {
        if (choice == "1") {
            quote();
        }
        else if (choice == "2") {
            inventoryObject->displayItems();
        }
        else if (choice == "3") {
            inventoryObject->removeUnits();
            inventoryObject->updateFile();
        }
        else if (choice == "4") {
            inventoryObject->addUnits();
            inventoryObject->updateFile();
        }
        else {
            fmt::print("Unknown option, please try again or enter \'q\' to quit\n");
        }
        choice = get_string_lower("\n1: Quote\n"
                                  "2: Display portfolio\n"
                                  "3: Remove units/shares\n"
                                  "4: Add units/shares\n"
                                  "> ");
    }
}

bool in(const std::vector<std::string>& arr, const std::string& target) {
    for (const std::string& s : arr) {
        if (s == target) {
            return true;
        }
    }
    return false;
}
