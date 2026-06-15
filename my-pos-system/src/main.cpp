
#include "DatabaseManager.h"
#include "BillingSystem.h"
#include "User.h"
#include "Utilities.h"
#include <memory>

class SessionTracker {
private:
    std::chrono::steady_clock::time_point lastActivityTime;
    const int TIMEOUT_MINUTES = 15;

public:
    SessionTracker() { refreshActivity(); }
    void refreshActivity() { lastActivityTime = std::chrono::steady_clock::now(); }
    bool isExpired() {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::minutes>(now - lastActivityTime).count();
        return elapsed >= TIMEOUT_MINUTES;
    }
};

bool authenticateUser(DatabaseManager& db, std::string& userRole, std::string& sessionUser) {
    Utils::clearScreen();
    std::cout << "=== CRYPTO GATEWAY LOG PORTAL ===\nEnter Authorized Username: ";
    std::string user; std::getline(std::cin, user);
    std::cout << "Enter Associated Password: ";
    std::string rawPass = Utils::getHiddenPassword();
    std::string hashed = Utils::sha256(rawPass); 

    if (db.verifyUser(user, hashed, userRole)) {
        sessionUser = user;
        return true;
    }
    return false;
}

void handleInventoryAdmin(DatabaseManager& db) {
    Utils::clearScreen();
    std::cout << "1. Create/Overwrite New Product Resource\n2. Patch Existing Product Field Variable\nChoice: ";
    int ch; std::cin >> ch; Utils::clearInput();
    
    if (ch == 1) {
        std::string id, name, cat, supp;
        double pr;
        int qty;
        std::cout << "Barcode/ID (Non-Empty): "; std::getline(std::cin, id);
        std::cout << "Product Name: "; std::getline(std::cin, name);
        std::cout << "Category Name: "; std::getline(std::cin, cat);
        
        std::cout << "Price Point ($ >= 0): ";
        std::string priceStr; std::getline(std::cin, priceStr);
        try {
            pr = std::stod(priceStr);
        } catch (...) {
            std::cout << "[ERROR]: Invalid price format.\n"; Utils::customSleep(2000); return;
        }
        
        std::cout << "Store Stock Volume (>= 0): ";
        std::string qtyStr; std::getline(std::cin, qtyStr);
        try {
            qty = std::stoi(qtyStr);
        } catch (...) {
            std::cout << "[ERROR]: Invalid quantity format.\n"; Utils::customSleep(2000); return;
        }
        
        std::cout << "Supplier Name: "; std::getline(std::cin, supp);
        
        if (id.empty() || name.empty() || pr < 0 || qty < 0) {
            std::cout << "[CRITICAL ERROR]: Validation Constraint Violated. Row rejected!" << std::endl;
            Utils::customSleep(2000); return;
        }
        
        if (db.addOrUpdateProduct(Product(id, name, cat, pr, qty, supp))) {
            std::cout << "Row added to table securely." << std::endl;
        } else {
            std::cout << "[ERROR]: Insertion rejected. Product name might already exist!" << std::endl;
        }
        Utils::customSleep(1500);
    } 
    else if (ch == 2) {
        std::string id; std::cout << "Barcode Target ID: "; std::getline(std::cin, id);
        std::cout << "Select Target Metadata (1. Name | 2. Price | 3. Qty): ";
        int field; std::cin >> field; Utils::clearInput();
        std::cout << "Enter Safe Normalized String Value: ";
        std::string val; std::getline(std::cin, val);
        
        if (db.editProductMetadata(id, field, val)) {
            std::cout << "Metadata updated successfully." << std::endl;
        } else {
            std::cout << "[ERROR]: Update failed (Possible DB check constraints violation, duplicate name, or missing ID)." << std::endl;
        }
        Utils::customSleep(2000);
    }
}

int main() {
    try {
        DatabaseManager db;
        BillingSystem bs;
        std::unique_ptr<User> currentUser = nullptr;

        while (true) {
            std::string role = "", userSession = "";
            if (!authenticateUser(db, role, userSession)) {
                std::cout << "[SECURITY WARNING]: Authentication Failure Event Logged." << std::endl;
                Utils::customSleep(2000); continue;
            }

            if (role == "Admin") currentUser = std::make_unique<Admin>(userSession);
            else currentUser = std::make_unique<Cashier>(userSession);

            SessionTracker session;
            bool activeSession = true;

            while (activeSession) {
                if (session.isExpired()) { 
                    std::cout << "\n[SECURITY DISCONNECT]: Terminal session expired via inactive context timeout." << std::endl;
                    Utils::customSleep(3000); activeSession = false; break;
                }

                Utils::clearScreen();
                currentUser->displayMenu();
                int choice; std::cin >> choice; Utils::clearInput();
                session.refreshActivity();

                if (currentUser->getRole() == "Admin") {
                    if (choice == 1) {
                        handleInventoryAdmin(db);
                    } else if (choice == 2) {
                        std::string id; std::cout << "Enter target removal Barcode: "; std::getline(std::cin, id);
                        std::cout << "Are you absolutely sure? (1=Confirm / 0=Abort): ";
                        int confirm; std::cin >> confirm; Utils::clearInput(); 
                        if(confirm == 1) {
                            if (db.deleteProduct(id)) {
                                std::cout << "[SUCCESS]: Product expunged correctly from storage." << std::endl;
                            } else {
                                std::cout << "[ERROR]: Expunge rejected! ID not found or linked to historic active invoices." << std::endl;
                            }
                            Utils::customSleep(3000);
                        }
                    } else if (choice == 3) {
                        Utils::clearScreen();
                        std::cout << "Sort options (id / name / price / qty): ";
                        std::string criteria; std::getline(std::cin, criteria);
                        auto list = db.getAllProducts(criteria);
                        std::cout << "\n--- CORE STORE ARCHIVE DIRECTORY ---\n";
                        for (const auto& p : list) {
                            std::cout << p.getId() << " | " << p.getName() << " | $" << p.getPrice() << " | Balance: " << p.getQuantity() << " units";
                            if(p.getQuantity() == 0) std::cout << " -> [CRITICAL: OUT OF STOCK]";
                            else if(p.getQuantity() < 5) std::cout << " -> [WARNING: LOW STOCK ALERT]";
                            std::cout << "\n";
                        }
                        std::cout << "\nPress Enter to return..."; std::cin.get();
                    } else if (choice == 4) {
                        db.printFinancialReport();
                        std::cout << "\nPress Enter to return..."; std::cin.get();
                    } else if (choice == 5) {
                        activeSession = false;
                    }
                } else { 
                    if (choice == 1) {
                        bs.executeInvoiceSession(db);
                    } else if (choice == 2) {
                        Utils::clearScreen();
                        auto list = db.getAllProducts("name");
                        for (const auto& p : list) {
                            std::cout << p.getName() << " \t Unit Cost: $" << p.getPrice() << " \t Units In Store: " << p.getQuantity();
                            if(p.getQuantity() < 5) std::cout << " [LOW STOCK]";
                            std::cout << "\n";
                        }
                        std::cout << "\nPress Enter to return..."; std::cin.get();
                    } else if (choice == 3) {
                        activeSession = false;
                    }
                }
            }
            currentUser.reset();
        }
    } catch (const std::exception& e) {
        std::cerr << "Uncaught execution fault traced from runtime: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
