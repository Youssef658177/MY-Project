
#include "BillingSystem.h"
#include "Utilities.h"
#include <iomanip>
#include <stdexcept>

void BillingSystem::executeInvoiceSession(DatabaseManager& db) {
    cart.clear();
    bool active = true;

    while (active) {
        Utils::clearScreen();
        std::cout << "--- POINT OF SALE SESSION ---\nEnter Product ID/Barcode (or '0' to checkout, '-1' to cancel): ";
        std::string input; std::getline(std::cin, input);

        if (input == "0") break;
        if (input == "-1") { cart.clear(); return; }

        Product p;
        if (db.getProductById(input, p)) {
            if (p.getQuantity() <= 0) {
                std::cout << "[NOTICE]: Item status: Out Of Stock!" << std::endl;
                Utils::customSleep(1500); continue;
            }
            std::cout << "Target item matched: " << p.getName() << " | Store Balance: " << p.getQuantity() << " units | Price: $" << p.getPrice() << "\nEnter Sales Volume Quantity: ";
            int qty; std::cin >> qty; Utils::clearInput();

            if (qty <= 0 || qty > p.getQuantity()) {
                std::cout << "[ERROR]: Quantity validation limit error." << std::endl;
                Utils::customSleep(1500); continue;
            }
            cart.push_back({p, qty, p.getPrice() * qty});
            std::cout << "Item packed to cart buffer snapshot." << std::endl;
            Utils::customSleep(1000);
        } else {
            std::cout << "[ERROR]: Unrecognized item barcode identifier." << std::endl;
            Utils::customSleep(1500);
        }
    }

    if (cart.empty()) return;

    double subTotal = 0;
    for (const auto& item : cart) subTotal += item.subtotal;

    std::cout << "Apply Custom Customer Discount Rate % (0 - 100): ";
    double discountRate; std::cin >> discountRate;
    if(discountRate < 0 || discountRate > 100) discountRate = 0;

    double discountAmt = subTotal * (discountRate / 100.0);
    double taxRate = 0.14;
    double taxAmt = (subTotal - discountAmt) * taxRate;
    double grandTotal = (subTotal - discountAmt) + taxAmt;

    std::cout << "Select Payment Gateway Protocol (1. Cash | 2. Card): ";
    int pCh; std::cin >> pCh; Utils::clearInput();
    std::string method = (pCh == 2) ? "Card" : "Cash";

    std::string timeString = Utils::getCurrentDateTime();

    db.beginImmediateTransaction();
    try {
        int invId = db.createInvoiceRecord(timeString, subTotal, taxAmt, discountAmt, grandTotal, method);
        for (auto& item : cart) {
            if (!db.updateProductStockAtomic(item.prod.getId(), item.qty)) {
                throw std::runtime_error("Insufficient stock or Concurrency Conflict for: " + item.prod.getName());
            }
            db.createInvoiceItem(invId, item.prod.getId(), item.qty, item.subtotal);
        }
        db.commitTransaction(); 
        Utils::logEvent("INFO", "Invoice deployed securely. ID: " + std::to_string(invId));
        
        Utils::clearScreen();
        std::cout << "\n========================================\n\tOFFICIAL FISCAL INVOICE\n========================================" << std::endl;
        std::cout << "Invoice ID Reference: INV-" << invId << "  | Timestamp: " << timeString << std::endl;
        std::cout << "Settlement Gateway  : " << method << std::endl;
        std::cout << "----------------------------------------" << std::endl;
        for (const auto& item : cart) {
            std::cout << item.prod.getName() << " \t x" << item.qty << " \t $" << item.subtotal << std::endl;
        }
        std::cout << "----------------------------------------" << std::endl;
        std::cout << "Gross Subtotal      : $" << subTotal << std::endl;
        std::cout << "Campaign Discount   : -$" << discountAmt << std::endl;
        std::cout << "VAT Tax Layer (14%) : $" << taxAmt << std::endl;
        std::cout << "Grand Total Payable : $" << grandTotal << "\n========================================" << std::endl;
    } catch (const std::exception& e) {
        db.rollbackTransaction(); 
        Utils::logEvent("CRITICAL", std::string("Transaction Rolled Back. Reason: ") + e.what());
        std::cout << "[TRANSACTION FAILURE]: " << e.what() << "\nSession rolled back cleanly." << std::endl;
    }

    std::cout << "\nPress Enter to return to point desk menu...";
    std::cin.get();
}
