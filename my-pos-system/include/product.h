
#ifndef PRODUCT_H
#define PRODUCT_H

#include <string>

class Product {
private:
    std::string id;
    std::string name;
    std::string category;
    double price;
    int quantity;
    std::string supplier;

public:
    Product() : price(0.0), quantity(0) {}
    Product(std::string i, std::string n, std::string c, double p, int q, std::string s)
        : id(i), name(n), category(c), price(p), quantity(q), supplier(s) {}

    std::string getId() const { return id; }
    std::string getName() const { return name; }
    std::string getCategory() const { return category; }
    double getPrice() const { return price; }
    int getQuantity() const { return quantity; }
    std::string getSupplier() const { return supplier; }

    void setId(std::string i) { id = i; }
    void setName(std::string n) { name = n; }
    void setCategory(std::string c) { category = c; }
    void setPrice(double p) { price = p; }
    void setQuantity(int q) { quantity = q; }
    void setSupplier(std::string s) { supplier = s; }
};

#endif
