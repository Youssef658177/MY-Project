
#ifndef BILLINGSYSTEM_H
#define BILLINGSYSTEM_H

#include "DatabaseManager.h"
#include <vector>

struct CartItem {
    Product prod;
    int qty;
    double subtotal;
};

class BillingSystem {
private:
    std::vector<CartItem> cart;
public:
    void executeInvoiceSession(DatabaseManager& db);
};

#endif
