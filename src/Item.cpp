#include <fmt/core.h>
#include <fmt/color.h>
#include "../include/Item.h"
#include "../include/Result.h"
#include "../include/get_quote.h"

Item::Item(std::string name, std::string type, std::string symbol, double avgPrice, double volume) {
    this->name = std::move(name);
    this->type = std::move(type);
    this->symbol = std::move(symbol);
    this->avgPrice = avgPrice;
    this->volume = volume;
}

double Item::display(int longestName, int longestSymbol, size_t longestProfitLoss) const {
    std::string tempSymbol = fmt::format("({})", symbol);

    // values are retrieved early to avoid unnecessary API requests
    double pL = profitLoss();
    double pLPercent = (pL != 0.0) ? pL / totalValuePrevious() * 100 : 0.0;
    double latest = latestPrice();
    auto color = (pL < 0) ? fmt::color::crimson : fmt::color::green; // green for positive values (gains) and red for negative values (losses)
    auto fgText = fg(fmt::color::antique_white);

    fmt::print(fgText, "{:<{}} - {:<{}} - $ ", name, longestName, tempSymbol, longestSymbol); // similar functionality of python's f-strings
    fmt::print(fg(color), "{:>+{}.2f} ({:+.2f}%)", pL, longestProfitLoss, pLPercent); // < and > are basically setw() and left() or right() in <iomanip>
    fmt::print(fgText, " - [Latest: ${} with {} units]\n", latest, volume);

    return pL;
}

double Item::profitLossPercent() const {
    if (totalValuePrevious() != 0.0) {
        return profitLoss() / totalValuePrevious();
    }
    return 0.0;
}

double Item::profitLoss() const {
    return totalValue() - totalValuePrevious();
}

double Item::latestPrice() const {
    return request(symbol, type)->get_number("latestPrice");
}

double Item::totalValue() const {
    return volume * latestPrice();
}

double Item::totalValuePrevious() const {
    return volume * avgPrice;
}

void Item::removeUnits(double units) {
    if (units > volume) {
        fmt::print("\nCannot subtract {} units of {} from the {} present\n", units, symbol, volume);
    }
    else {
        volume -= units;
        fmt::print("\nSuccessfully removed {} units from {}\n", units, symbol);
    }
}

void Item::addUnits(double units) {
    double totalValue = totalValuePrevious();
    double totalValueLatest = latestPrice() * units;

    volume += units;
    avgPrice = (totalValue + totalValueLatest) / volume;
}

std::string Item::str() const {
    return fmt::format("{},{},{},{:.2f},{}\n", name, type, symbol, avgPrice, volume);
}

std::string Item::getName() const {
    return name;
}

std::string Item::getType() const {
    return type;
}

std::string Item::getSymbol() const {
    return symbol;
}
