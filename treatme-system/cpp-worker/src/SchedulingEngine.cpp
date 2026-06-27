#include <iostream>
#include <queue>
#include <vector>
#include <string>

// هيكل البيانات لطلب الحجز
struct BookingTask {
    std::string bookingId;
    int priority;   // 1 (حرج/عالي) إلى 5 (عادي)
    int deadline;   // الوقت الأقصى للتنفيذ (Unix Timestamp)

    // Operator Overloading لتحديد أولوية الطابور (EDF Algorithm)
    bool operator<(const BookingTask& other) const {
        // إذا كان الوقت متطابقاً، الأولوية للحالة الحرجة (الرقم الأقل)
        if (deadline == other.deadline) {
            return priority > other.priority; 
        }
        // بناء Min-Heap: الموعد الأقرب يخرج أولاً
        return deadline > other.deadline; 
    }
};

class SchedulingEngine {
private:
    std::priority_queue<BookingTask> taskQueue;

public:
    // إضافة حجز قادم من RabbitMQ
    void addBooking(const std::string& id, int priority, int deadline) {
        taskQueue.push({id, priority, deadline});
        std::cout << "[C++ Engine] 📥 Received & Scheduled Booking: " << id << "\n";
    }

    // معالجة الحجز ذو الأولوية القصوى
    void processNextBooking() {
        if (taskQueue.empty()) {
            std::cout << "[C++ Engine] 💤 No pending bookings.\n";
            return;
        }

        BookingTask nextTask = taskQueue.top();
        taskQueue.pop();
        
        std::cout << "[C++ Engine] ⚙️ Processing Booking ID: " << nextTask.bookingId 
                  << " | Priority: " << nextTask.priority 
                  << " | Deadline: " << nextTask.deadline << "\n";
        
        // هنا يتم إرسال النتيجة مرة أخرى إلى Java أو تحديث قاعدة البيانات
    }
};

int main() {
    SchedulingEngine engine;
    
    // محاكاة لطلبات قادمة من RabbitMQ بأوقات وأولويات مختلفة
    engine.addBooking("BKG-001", 3, 1700000500); // حجز عادي
    engine.addBooking("BKG-002", 1, 1700000200); // حالة حرجة ومستعجلة
    engine.addBooking("BKG-003", 2, 1700000200); // حالة متوسطة بنفس الوقت
    
    std::cout << "-----------------------------------\n";
    
    // المعالجة ستتم بناءً على خوارزمية EDF وليس ترتيب الدخول
    engine.processNextBooking(); // سيعالج BKG-002 أولاً
    engine.processNextBooking(); // ثم BKG-003
    engine.processNextBooking(); // ثم BKG-001
    
    return 0;
}
