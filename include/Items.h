#ifndef INVENTORY_ITEMS_H
#define INVENTORY_ITEMS_H

#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <iostream>
#include "Item.h"

class Items {
private:
    std::vector<Item*> inventoryItems;
    std::string filename;
    int longestName;
    int longestSymbol;

public:
    Items();
    ~Items();
    double totalValue() const;
    double totalValuePrevious() const;
    size_t longestProfitLoss() const;
    void removeUnits() const;
    void addUnits();
    void displayItems() const;
    void updateFile() const;
    static double sd(const std::string& str) ;
};

#endif //INVENTORY_ITEMS_H
