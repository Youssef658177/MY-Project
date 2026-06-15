#include "DatabaseManager.h"
#include "Utilities.h"
#include <stdexcept>
#include <iostream>

// Constructor: يفتح الاتصال بقاعدة البيانات ويفعل قيود العلاقات (Foreign Keys) ويُنشئ الجداول
DatabaseManager::DatabaseManager() {
    if (sqlite3_open("enterprise_market.db", &db) != SQLITE_OK) {
        Utils::logEvent("FATAL", "Database connection pipeline broken!");
        throw std::runtime_error("Fatal Error: Database connection pipeline broken!");
    }
    // تفعيل قيود المفاتيح الأجنبية لحماية سلامة البيانات
    sqlite3_exec(db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);
    initializeTablesAndIndexes();
}

// Destructor: يغلق الاتصال بأمان عند تدمير كائن الكلاس
DatabaseManager::~DatabaseManager() {
    sqlite3_close(db);
}

// التحكم بالـ Transactions بشكل فوري (Immediate) لمنع الـ Deadlocks في البيئات المتزامنة
void DatabaseManager::beginImmediateTransaction() { 
    sqlite3_exec(db, "BEGIN IMMEDIATE TRANSACTION;", nullptr, nullptr, nullptr); 
}
void DatabaseManager::commitTransaction() { sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr); }
void DatabaseManager::rollbackTransaction() { sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr); }

// تهيئة جداول قاعدة البيانات، الفهارس (Indexes)، والحسابات الافتراضية المحمية بـ SHA-256
void DatabaseManager::initializeTablesAndIndexes() {
    const char* prodTable = "CREATE TABLE IF NOT EXISTS products("
                           "id TEXT PRIMARY KEY, name TEXT UNIQUE, cat TEXT, "
                           "price REAL CHECK(price >= 0), "
                           "qty INTEGER CHECK(qty >= 0), "
                           "supplier TEXT);";
                           
    const char* userTable = "CREATE TABLE IF NOT EXISTS users(username TEXT PRIMARY KEY, password TEXT, role TEXT);";
    const char* invTable = "CREATE TABLE IF NOT EXISTS invoices(id INTEGER PRIMARY KEY AUTOINCREMENT, datetime TEXT, subtotal REAL, tax REAL, discount REAL, grand_total REAL, payment TEXT);";
    const char* invItems = "CREATE TABLE IF NOT EXISTS invoice_items("
                           "invoice_id INTEGER, prod_id TEXT, qty INTEGER CHECK(qty > 0), line_total REAL, "
                           "FOREIGN KEY(invoice_id) REFERENCES invoices(id) ON DELETE CASCADE, "
                           "FOREIGN KEY(prod_id) REFERENCES products(id) ON DELETE RESTRICT);";

    sqlite3_exec(db, prodTable, nullptr, nullptr, nullptr);
    sqlite3_exec(db, userTable, nullptr, nullptr, nullptr);
    sqlite3_exec(db, invTable, nullptr, nullptr, nullptr);
    sqlite3_exec(db, invItems, nullptr, nullptr, nullptr);

    // إنشاء الفهارس لتسريع عمليات البحث والاستعلام
    sqlite3_exec(db, "CREATE INDEX IF NOT EXISTS idx_prod_name ON products(name);", nullptr, nullptr, nullptr);
    sqlite3_exec(db, "CREATE INDEX IF NOT EXISTS idx_invoice_date ON invoices(datetime);", nullptr, nullptr, nullptr);

    // توليد الهاش الافتراضي لكلمة المرور لحساب الـ Admin والـ Cashier
    std::string adminHash = Utils::sha256("admin123");
    std::string cashierHash = Utils::sha256("cashier123");
    
    std::string insertAdmin = "INSERT OR IGNORE INTO users VALUES('admin', '" + adminHash + "', 'Admin');";
    std::string insertCashier = "INSERT OR IGNORE INTO users VALUES('cashier', '" + cashierHash + "', 'Cashier');";
    
    sqlite3_exec(db, insertAdmin.c_str(), nullptr, nullptr, nullptr);
    sqlite3_exec(db, insertCashier.c_str(), nullptr, nullptr, nullptr);
}

// دالة التحقق من المستخدم وإرجاع صلاحياته (مع حماية النص المنسوخ من الذاكرة المؤقتة)
bool DatabaseManager::verifyUser(const std::string& user, const std::string& hashedPass, std::string& role) {
    const char* query = "SELECT role FROM users WHERE username = ? AND password = ?;";
    sqlite3_stmt* rawStmt;
    bool success = false;
    
    if (sqlite3_prepare_v2(db, query, -1, &rawStmt, nullptr) == SQLITE_OK) {
        SQLiteStatement stmt(rawStmt); 
        sqlite3_bind_text(stmt, 1, user.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, hashedPass.c_str(), -1, SQLITE_TRANSIENT);
        
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            // صب النص داخل std::string فوراً لنسخه وحمايته قبل تدمير الـ statement
            role = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
            success = true;
            Utils::logEvent("INFO", "User authenticated: " + user);
        } else {
            Utils::logEvent("WARN", "Authentication failed for user: " + user);
        }
    }
    return success;
}

// إضافة منتج جديد أو تحديث بياناته بالكامل في حالة وجوده مسبقاً (Upsert)
bool DatabaseManager::addOrUpdateProduct(const Product& p) {
    const char* query = "INSERT INTO products VALUES(?,?,?,?,?,?) "
                        "ON CONFLICT(id) DO UPDATE SET "
                        "name=excluded.name, cat=excluded.cat, price=excluded.price, qty=excluded.qty, supplier=excluded.supplier;";
    sqlite3_stmt* rawStmt;
    bool success = false;
    
    if (sqlite3_prepare_v2(db, query, -1, &rawStmt, nullptr) == SQLITE_OK) {
        SQLiteStatement stmt(rawStmt);
        sqlite3_bind_text(stmt, 1, p.getId().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, p.getName().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, p.getCategory().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_double(stmt, 4, p.getPrice());
        sqlite3_bind_int(stmt, 5, p.getQuantity());
        sqlite3_bind_text(stmt, 6, p.getSupplier().c_str(), -1, SQLITE_TRANSIENT);
        
        if (sqlite3_step(stmt) == SQLITE_DONE) {
            success = true;
        } else {
            Utils::logEvent("ERROR", "Failed to add/update product: " + getLastError());
        }
    }
    return success;
}

// التحديث الذري والآمن للمخزون (الحل العبقري ضد الـ Race Conditions)
bool DatabaseManager::updateProductStockAtomic(const std::string& id, int qtyToSubtract) {
    const char* query = "UPDATE products SET qty = qty - ? WHERE id = ? AND qty >= ?;";
    sqlite3_stmt* rawStmt;
    bool success = false;
    
    if (sqlite3_prepare_v2(db, query, -1, &rawStmt, nullptr) == SQLITE_OK) {
        SQLiteStatement stmt(rawStmt);
        sqlite3_bind_int(stmt, 1, qtyToSubtract);
        sqlite3_bind_text(stmt, 2, id.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 3, qtyToSubtract);
        
        sqlite3_step(stmt);
        // التحقق مما إذا تأثر أي سطر (إذا لم يتأثر، يعني ذلك أن الكمية المتوفرة أقل من المطلوبة)
        if (sqlite3_changes(db) > 0) {
            success = true;
        }
    }
    return success;
}

// تعديل حقل معين لمنتج موجود مسبقاً مع التحقق من صحة المدخلات الرقمية
bool DatabaseManager::editProductMetadata(const std::string& id, int fieldChoice, const std::string& newValue) {
    std::string field;
    switch (fieldChoice) {
        case 1: field = "name"; break;
        case 2: field = "price"; break;
        case 3: field = "qty"; break;
        default: return false;
    }
    std::string query = "UPDATE products SET " + field + " = ? WHERE id = ?;";
    sqlite3_stmt* rawStmt;
    bool result = false;
    
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &rawStmt, nullptr) == SQLITE_OK) {
        SQLiteStatement stmt(rawStmt);
        try {
            if (fieldChoice == 1) {
                sqlite3_bind_text(stmt, 1, newValue.c_str(), -1, SQLITE_TRANSIENT);
            } else if (fieldChoice == 2) {
                double val = std::stod(newValue);
                sqlite3_bind_double(stmt, 1, val);
            } else {
                int val = std::stoi(newValue);
                sqlite3_bind_int(stmt, 1, val);
            }
            sqlite3_bind_text(stmt, 2, id.c_str(), -1, SQLITE_TRANSIENT);
            
            if (sqlite3_step(stmt) == SQLITE_DONE) {
                result = (sqlite3_changes(db) > 0); 
            }
        } catch (...) {
            Utils::logEvent("ERROR", "Metadata parsing violation for field choice: " + std::to_string(fieldChoice));
            return false;
        }
    }
    if (!result) Utils::logEvent("ERROR", "Metadata update query failed: " + getLastError());
    return result;
}

// حذف منتج من المخزن بشكل كامل بالاعتماد على الـ ID
bool DatabaseManager::deleteProduct(const std::string& id) {
    const char* query = "DELETE FROM products WHERE id = ?;";
    sqlite3_stmt* rawStmt;
    bool success = false;
    
    if (sqlite3_prepare_v2(db, query, -1, &rawStmt, nullptr) == SQLITE_OK) {
        SQLiteStatement stmt(rawStmt);
        sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_step(stmt);
        
        if (sqlite3_changes(db) > 0) {
            success = true;
            Utils::logEvent("INFO", "Product resource expunged: " + id);
        } else {
            Utils::logEvent("WARN", "Delete operation requested for non-existent product ID: " + id);
        }
    }
    return success;
}

// جلب منتج معين للتحقق منه إما عن طريق الـ ID أو الاسم (Case-Insensitive)
bool DatabaseManager::getProductById(const std::string& id, Product& p) {
    const char* query = "SELECT * FROM products WHERE UPPER(id) = UPPER(?) OR UPPER(name) = UPPER(?);";
    sqlite3_stmt* rawStmt;
    bool found = false;
    
    if (sqlite3_prepare_v2(db, query, -1, &rawStmt, nullptr) == SQLITE_OK) {
        SQLiteStatement stmt(rawStmt);
        sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, id.c_str(), -1, SQLITE_TRANSIENT);
        
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            p.setId(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
            p.setName(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
            p.setCategory(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
            p.setPrice(sqlite3_column_double(stmt, 3));
            p.setQuantity(sqlite3_column_int(stmt, 4));
            p.setSupplier(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5)));
            found = true;
        }
    }
    return found;
}

// جلب جميع المنتجات في المخزن مع إمكانية الترتيب الديناميكي والآمن ضد الـ SQL Injection
std::vector<Product> DatabaseManager::getAllProducts(const std::string& sortBy) {
    std::string safeOrderField = "id";
    if (sortBy == "name" || sortBy == "price" || sortBy == "qty") {
        safeOrderField = sortBy;
    }
    std::vector<Product> list;
    std::string query = "SELECT * FROM products ORDER BY " + safeOrderField + " ASC;";
    sqlite3_stmt* rawStmt;
    
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &rawStmt, nullptr) == SQLITE_OK) {
        SQLiteStatement stmt(rawStmt);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Product p(
                reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)),
                reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)),
                reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)),
                sqlite3_column_double(stmt, 3),
                sqlite3_column_int(stmt, 4),
                reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5))
            );
            list.push_back(p);
        }
    }
    return list;
}

// إنشاء سجل الفاتورة الرئيسي في جدول الـ invoices وإرجاع الـ ID المولد تلقائياً
int DatabaseManager::createInvoiceRecord(const std::string& datetime, double subtotal, double tax, double discount, double grandTotal, const std::string& payment) {
    const char* query = "INSERT INTO invoices(datetime, subtotal, tax, discount, grand_total, payment) VALUES(?,?,?,?,?,?);";
    sqlite3_stmt* rawStmt;
    int id = 0;
    
    if (sqlite3_prepare_v2(db, query, -1, &rawStmt, nullptr) == SQLITE_OK) {
        SQLiteStatement stmt(rawStmt);
        sqlite3_bind_text(stmt, 1, datetime.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_double(stmt, 2, subtotal);
        sqlite3_bind_double(stmt, 3, tax);
        sqlite3_bind_double(stmt, 4, discount);
        sqlite3_bind_double(stmt, 5, grandTotal);
        sqlite3_bind_text(stmt, 6, payment.c_str(), -1, SQLITE_TRANSIENT);
        
        if (sqlite3_step(stmt) == SQLITE_DONE) {
            id = static_cast<int>(sqlite3_last_insert_rowid(db));
        } else {
            throw std::runtime_error("Database Failure: Invoice registration failed -> " + getLastError());
        }
    }
    return id;
}

// إضافة العناصر المنبثقة من الفاتورة داخل جدول الـ invoice_items لربطها بالفاتورة الرئيسية
void DatabaseManager::createInvoiceItem(int invoiceId, const std::string& prodId, int qty, double lineTotal) {
    const char* query = "INSERT INTO invoice_items VALUES(?,?,?,?);";
    sqlite3_stmt* rawStmt;
    
    if (sqlite3_prepare_v2(db, query, -1, &rawStmt, nullptr) == SQLITE_OK) {
        SQLiteStatement stmt(rawStmt);
        sqlite3_bind_int(stmt, 1, invoiceId);
        sqlite3_bind_text(stmt, 2, prodId.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 3, qty);
        sqlite3_bind_double(stmt, 4, lineTotal);
        
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            throw std::runtime_error("Database Failure: Invoice item insertion aborted -> " + getLastError());
        }
    }
}

// طباعة التقرير المالي التراكمي للأرباح وعدد الفواتير المصدرة من النظام
void DatabaseManager::printFinancialReport() {
    const char* query = "SELECT SUM(grand_total), COUNT(id) FROM invoices;";
    sqlite3_stmt* rawStmt;
    double revenue = 0; int count = 0;
    
    if (sqlite3_prepare_v2(db, query, -1, &rawStmt, nullptr) == SQLITE_OK) {
        SQLiteStatement stmt(rawStmt);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            revenue = sqlite3_column_double(stmt, 0);
            count = sqlite3_column_int(stmt, 1);
        }
    }
    std::cout << "\n=== SYSTEM FINANCIAL SUMMARY ===\nCumulative Net Revenue Volume: $" << revenue 
              << "\nTotal Deployed Invoice Count: " << count << std::endl;
}
