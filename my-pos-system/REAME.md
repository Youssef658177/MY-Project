
# Enterprise Point of Sale (POS) & Inventory Management System

A Production-Grade, highly secure, and concurrent Enterprise POS and Inventory Management System engineered in **Modern C++ (C++17)** and backed by an optimized **SQLite3** relational database database layer. 

This architecture has been meticulously refactored to eliminate common systems programming pitfalls, featuring thread-safe operations, proper cryptographic primitives, and atomic hardware-level/database-level transactions.

---

## 🏗️ Architectural Blueprint & Repository Structure

The project strictly follows the industry standard decoupling paradigm, splitting the abstract interfaces (`include/`) from concrete implementation runtime binaries (`src/`):

```text
my-pos-system/
│
├── .gitignore               # Excludes compilation binaries, databases, and temporary logs.
├── README.md                # System documentation and structural audit overview.
├── CMakeLists.txt           # Cross-platform CMake construction script.
│
├── include/                 # Declarations & Interface Headers (.h)
│   ├── DatabaseManager.h    # RAII SQLite wrapper and atomic transaction engines.
│   ├── BillingSystem.h      # Core POS checkout loop context logic.
│   ├── Product.h            # Data model representation encapsulated objects.
│   ├── User.h               # Role-Based Access Control (RBAC) Polymorphic classes.
│   └── Utilities.h          # Thread-safe utilities, SHA-256, and platform abstractions.
│
└── src/                     # Implementation Source Files (.cpp)
    ├── main.cpp             # Central bootstrapper and Session lifecycle tracking.
    ├── DatabaseManager.cpp  # Prepared statements, schemas, and relational constraints.
    └── BillingSystem.cpp    # Isolated transactional execution contexts.
