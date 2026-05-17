#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>

using namespace std;

// تعريف الكلاس Bill لتخزين بيانات المنتجات [cite: 3]
class Bill {
private:
    string Item;
    int Rate, Quantity;

public:
    // المشيد (Constructor) لتهيئة القيم [cite: 4, 5]
    Bill() {
        Item = "";
        Rate = 0;
        Quantity = 0;
    }

    // دالات الضبط (Setter Functions) [cite: 7, 8, 9, 10]
    void setItem(string item) {
        Item = item;
    }

    void setRate(int rate) {
        Rate = rate;
    }

    void setQuant(int quant) {
        Quantity = quant;
    }

    // دالات الجلب (Getter Functions) [cite: 11, 13, 14, 15]
    string getItem() {
        return Item;
    }

    int getRate() {
        return Rate;
    }

    int getQuant() {
        return Quantity;
    }
};

// دالة مساعدة للانتظار متوافقة مع المتصفح [cite: 39]
void customSleep(int milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

// وظيفة إضافة صنف جديد إلى الملف [cite: 21]
void addItem(Bill b) {
    bool close = false;
    while (!close) {
        system("clear"); // [cite: 17, 39]
        int choice;
        cout << "\t1. Add Item" << endl;
        cout << "\t2. Close" << endl;
        cout << "\tEnter Choice: ";
        cin >> choice;

        if (choice == 1) {
            system("clear");
            string item;
            int rate, quant;

            cout << "\tEnter Item Name: ";
            cin >> item;
            b.setItem(item);

            cout << "\tEnter Rate of Item: ";
            cin >> rate;
            b.setRate(rate);

            cout << "\tEnter Total Quantity: ";
            cin >> quant;
            b.setQuant(quant);

            // فتح ملف للحفظ [cite: 31, 32, 33]
            ofstream out("bill.txt", ios::app);
            if (!out) {
                cout << "\tError: File Can't Open!" << endl; // [cite: 34]
            } 
            else {
                out << b.getItem() << " : " << b.getRate() << " : " << b.getQuant() << endl; // [cite: 35, 36, 37]
                cout << "\tItem Added Successfully!" << endl; // [cite: 38]
            }
            out.close(); // [cite: 38]
            customSleep(3000); // [cite: 39]
        } 
        else if (choice == 2) {
            close = true; // [cite: 40]
            cout << "\tBack to Main Menu" << endl; // [cite: 41]
            customSleep(3000);
        }
    }
}

// وظيفة طباعة الفاتورة ومعالجة المشتريات [cite: 45]
void printBill() {
    system("clear");
    int count = 0; // [cite: 46]
    bool close = false;
    
    while (!close) {
        system("clear"); // [cite: 47, 48]
        int choice;
        cout << "\t1. Add Bill Item" << endl;
        cout << "\t2. Close Session" << endl;
        cout << "\tEnter Choice: ";
        cin >> choice;

        if (choice == 1) {
            string item;
            int quant;
            cout << "\tEnter Item: "; // [cite: 49]
            cin >> item;
            cout << "\tEnter Quantity: "; // [cite: 50]
            cin >> quant;

            ifstream in("bill.txt"); // [cite: 51]
            ofstream out("bill_temp.txt"); // [cite: 52]
            
            string line;
            bool found = false; // [cite: 53]

            while (getline(in, line)) { // [cite: 53]
                stringstream ss(line); // [cite: 54]
                string itemName;
                int itemRate, itemQuant;
                char delimiter; // [cite: 56]

                // قراءة البيانات من السطر [cite: 58]
                ss >> itemName >> delimiter >> itemRate >> delimiter >> itemQuant;

                if (item == itemName) { // [cite: 59]
                    found = true;
                    if (quant <= itemQuant) { // [cite: 61, 93]
                        int amount = itemRate * quant; // [cite: 62, 63]
                        cout << "\tItem | Rate | Quantity | Amount" << endl; // [cite: 64]
                        cout << "\t" << itemName << " | " << itemRate << " | " << quant << " | " << amount << endl; // [cite: 65, 66, 67]
                        
                        itemQuant -= quant; // تحديث الكمية [cite: 69, 70]
                        count += amount; // تحديث المجموع [cite: 71]
                    } 
                    else {
                        cout << "\tSorry, " << item << " quantity not available!" << endl; // [cite: 74, 75]
                    }
                }
                // كتابة الصنف (سواء تم تعديله أو لا) في الملف المؤقت [cite: 73, 76]
                out << itemName << " : " << itemRate << " : " << itemQuant << endl;
            }

            if (!found) {
                cout << "\tItem Not Available!" << endl; // [cite: 77, 78]
            }

            in.close(); // [cite: 79]
            out.close(); // [cite: 79]
            remove("bill.txt"); // [cite: 80]
            rename("bill_temp.txt", "bill.txt"); // [cite: 81]
            customSleep(3000);
        } 
        else if (choice == 2) {
            close = true; // [cite: 83]
            cout << "\tCounting Total Bill..." << endl;
            customSleep(3000);
        }
    }
    
    system("clear"); // [cite: 84]
    cout << "\tTotal Bill: " << count << endl; // [cite: 85]
    cout << "\tThanks for Shopping!" << endl; // [cite: 86]
    customSleep(5000); // [cite: 86]
}

// الدالة الرئيسية [cite: 15]
int main() {
    Bill b;
    bool exit = false;
    
    while (!exit) {
        system("clear"); // [cite: 17]
        int val;
        cout << "\n\tWelcome To Super Market Billing System" << endl; // [cite: 19]
        cout << "\t**************************************" << endl;
        cout << "\t1. Add Item" << endl; // [cite: 19]
        cout << "\t2. Print Bill" << endl; // [cite: 19]
        cout << "\t3. Exit" << endl; // [cite: 20]
        cout << "\tEnter Choice: ";
        cin >> val;

        if (val == 1) {
            addItem(b); // [cite: 42]
        } 
        else if (val == 2) {
            printBill(); // [cite: 45]
        } 
        else if (val == 3) {
            system("clear");
            exit = true; // [cite: 88]
            cout << "\tGood Luck!" << endl; // [cite: 88]
            customSleep(3000);
        }
    }
    
    return 0;
}
