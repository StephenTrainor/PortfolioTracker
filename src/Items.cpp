#include <fmt/core.h>
#include <fmt/color.h>
#include "../include/core.h"
#include "../include/Items.h"
#include "../include/get_quote.h"

static std::vector<std::string> get_lines(std::ifstream& f) {
    std::vector<std::string> lines;
    std::string line;

    getline(f, line);
    lines.push_back(line);

    while (!line.empty()) {
        getline(f, line);
        lines.push_back(line);
    }

    return lines;
}

Items::Items() {
    std::ifstream f;

    std::string file = get_string("\nEnter name of portfolio file (ends in .dat): ");
    f.open(file, std::ios::in | std::ios::binary);

    while (!f) {
        fmt::print("Could not open \'{}\', please try again.\n");

        file = get_string("Enter name of portfolio file (ends in .dat):");
        f.open(file, std::ios::in | std::ios::binary);
    }

    filename = file;

    for (const std::string& line : get_lines(f)) {
        if (line.empty()) {
            continue;
        }
        std::vector<std::string> data = split(line, ",");

        Item* temp = new Item(data[0], data[1], data[2], sd(data[3]), sd(data[4]));
        inventoryItems.push_back(temp);
    }

    longestName = 0; // used for nice alignment when displaying portfolio
    longestSymbol = 0;

    for (Item* item : inventoryItems) {
        if (item->getName().length() > longestName) {
            longestName = item->getName().length();
        }
        if (item->getSymbol().length() > longestSymbol) {
            longestSymbol = item->getSymbol().length();
        }
    }

    f.close();
}

Items::~Items() {
    for (Item* temp : inventoryItems) {
        delete temp; // avoid memory leaks
    }
}

double Items::totalValue() const { // might remove due to number of API calls needed is linear to size of portfolio
    double total = 0;

    for (Item* i : inventoryItems) {
        total += i->totalValue();
    }

    return total;
}

double Items::totalValuePrevious() const {
    double total = 0;

    for (Item* i : inventoryItems) {
        total += i->totalValuePrevious();
    }

    return total;
}

size_t Items::longestProfitLoss() const {
    size_t longest = 0;

    for (Item* i : inventoryItems) {
        size_t temp = dtos(i->profitLoss()).length();

        if (temp > longest) {
            longest = temp;
        }
    }

    return longest;
}

void Items::removeUnits() const {
    std::string symbol = get_string_upper("\nEnter asset's symbol: ");
    double units = get_double("Enter the number of units/shares to remove: ");

    while (units < 0) {
        fmt::print("Cannot remove a negative amount of shares.\n");

        units = get_double("Enter the number of units/shares to remove: ");
    }

    for (Item* i : inventoryItems) {
        if (i->getSymbol() == symbol) {
            i->removeUnits(units);
            return; // exit early to save a little time
        }
    }

    fmt::print("Could not find an asset in your portfolio with the symbol \'{}\'", symbol);
}

void Items::addUnits() {
    std::vector<std::string> typesAllowed = {"stock", "crypto"};
    std::string type = get_string_lower("\nEnter type of asset: "); // needed for API request later on

    while (!in(typesAllowed, type)) {
        fmt::print("\'{}\' is not an allowed asset type.\n", type);
        type = get_string_lower("Enter type of asset: ");
    }

    std::string symbol = get_string_upper("Enter symbol: ");
    double volume = get_double("Enter units to add: ");

    while (volume < 0) {
        fmt::print("Cannot add a negative amount of units to {}\n", symbol);

        volume = get_double("Enter units to add: ");
    }

    for (Item* i : inventoryItems) {
        if (i->getSymbol() == symbol && i->getType() == type) {
            i->addUnits(volume);
            return;
        }
    }

    // if there isn't any symbol to add units, a new item is simply added
    std::unique_ptr<Result> data = request(symbol, type);
    std::string nameTemp = " ";

    if (type == "stock") {
        nameTemp = data->get_data("companyName");
    }

    Item* temp = new Item(nameTemp, type, symbol, data->get_number("latestPrice"), volume);
    inventoryItems.push_back(temp);
}

void Items::displayItems() const {
    size_t longestPL = longestProfitLoss(); // API calls is linear to items in inventory
    double profit = 0;

    for (Item* i : inventoryItems) {
        profit += i->display(longestName, longestSymbol + 2, longestPL);
    }

    // calculating totals with as little API calls as possible
    double totalPrevious = totalValuePrevious();
    double profitPercentage = (totalPrevious != 0.0) ? profit / totalPrevious * 100 : 0.0;
    auto fgText = fg(fmt::color::antique_white);
    auto color = (profit < 0) ? fmt::color::crimson : fmt::color::green;

    fmt::print(fgText | fmt::emphasis::bold, "\n{:>{}} - $ ", "Total P/L",
               longestName + longestSymbol - (dtos(profit).length() - longestPL) + 4);
    fmt::print(fg(color), "{:+.2f} ({:+.2f}%)", profit, profitPercentage);
}

void Items::updateFile() const {
    std::ofstream file(filename, std::ios::out | std::ios::binary);

    for (Item* i : inventoryItems) {
        file << i->str(); // override file with same name
    }

    file.close();
}

double Items::sd(const std::string& str) {
    return std::stod(str);
}
