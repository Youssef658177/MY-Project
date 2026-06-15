
#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include "Product.h"
#include <sqlite3.h>
#include <vector>
#include <string>

class SQLiteStatement {
private:
    sqlite3_stmt* stmt;
public:
    SQLiteStatement(sqlite3_stmt* s) : stmt(s) {}
    ~SQLiteStatement() { if (stmt) sqlite3_finalize(stmt); }
    sqlite3_stmt* get() const { return stmt; }
    operator sqlite3_stmt*() const { return stmt; }
};

class DatabaseManager {
private:
    sqlite3* db;
    void initializeTablesAndIndexes();

public:
    DatabaseManager();
    ~DatabaseManager();

    void beginImmediateTransaction(); 
    void commitTransaction();
    void rollbackTransaction();

    bool addOrUpdateProduct(const Product& p);
    bool editProductMetadata(const std::string& id, int fieldChoice, const std::string& newValue);
    bool deleteProduct(const std::string& id);
    bool getProductById(const std::string& id, Product& p);
    bool updateProductStockAtomic(const std::string& id, int qtyToSubtract);

    std::vector<Product> getAllProducts(const std::string& sortBy = "id");
    bool verifyUser(const std::string& user, const std::string& hashedPass, std::string& role);
    
    int createInvoiceRecord(const std::string& datetime, double subtotal, double tax, double discount, double grandTotal, const std::string& payment);
    void createInvoiceItem(int invoiceId, const std::string& prodId, int qty, double lineTotal);
    void printFinancialReport();
    
    std::string getLastError() const { return sqlite3_errmsg(db); }
};

#endif
