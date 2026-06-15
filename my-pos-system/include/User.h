
#ifndef USER_H
#define USER_H

#include <iostream>
#include <string>

class User {
protected:
    std::string username;
    std::string role;
public:
    User(std::string u, std::string r) : username(u), role(r) {}
    virtual ~User() = default;
    virtual void displayMenu() = 0;
    std::string getRole() const { return role; }
    std::string getUsername() const { return username; }
};

class Admin : public User {
public:
    Admin(std::string u) : User(u, "Admin") {}
    void displayMenu() override {
        std::cout << "=== [ADMINISTRATOR CONTROL DESK] ===\n"
                  << "Active Session: " << username << "\n"
                  << "1. Inventory Control (Add/Update/Patch Product)\n"
                  << "2. Permanent Delete/Expunge Product Resource\n"
                  << "3. Display Core Stock Archive Directory\n"
                  << "4. Generate Cumulative Financial Audit Summary\n"
                  << "5. Terminate Terminal Session (Logout)\n"
                  << "Select Authorized Command Axis: ";
    }
};

class Cashier : public User {
public:
    Cashier(std::string u) : User(u, "Cashier") {}
    void displayMenu() override {
        std::cout << "=== [CASHIER POINT OF SALE DESK] ===\n"
                  << "Active Operational Node: " << username << "\n"
                  << "1. Launch New Point Of Sale Billing Session\n"
                  << "2. Query Available Store Items Stock\n"
                  << "3. Terminate Terminal Session (Logout)\n"
                  << "Select Authorized Command Axis: ";
    }
};

#endif
