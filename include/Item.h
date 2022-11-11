#ifndef INVENTORY_ITEM_H
#define INVENTORY_ITEM_H

#include <iostream>
#include <string>

class Item {
private:
    std::string name;
    std::string type;
    std::string symbol;
    double avgPrice;
    double volume;

public:
    Item(std::string name, std::string type, std::string symbol, double avgPrice, double volume = 0.0);
    double display(int longestName, int longestSymbol, size_t longestProfitLoss) const;
    double profitLossPercent() const;
    double profitLoss() const;
    double latestPrice() const;
    double totalValue() const;
    double totalValuePrevious() const;
    void removeUnits(double units);
    void addUnits(double units);
    std::string str() const;
    std::string getName() const;
    std::string getType() const;
    std::string getSymbol() const;
};

#endif //INVENTORY_ITEM_H
