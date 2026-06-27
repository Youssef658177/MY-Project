from flask import Flask, jsonify
import matplotlib.pyplot as plt
import io
import base64

app = Flask(__name__)

@app.route('/api/analytics/bookings', methods=['GET'])
def get_booking_stats():
    # محاكاة لجلب البيانات من قاعدة البيانات
    labels = ['Completed', 'Pending', 'Cancelled']
    sizes = [65, 25, 10]  # النسب المئوية للحجوزات
    colors = ['#1fa3a3', '#f5c93a', '#e74c3c']

    # إنشاء الرسم البياني
    fig, ax = plt.subplots()
    ax.pie(sizes, labels=labels, colors=colors, autopct='%1.1f%%', startangle=90)
    ax.axis('equal')  # لضمان خروج الرسم بشكل دائري متناسق

    # حفظ الرسم في الذاكرة (Memory Buffer) بدلاً من القرص الصلب
    buf = io.BytesIO()
    plt.savefig(buf, format='png', transparent=True)
    buf.seek(0)
    
    # تحويل الصورة إلى تشفير Base64 لإرسالها عبر HTTP
    img_base64 = base64.b64encode(buf.read()).decode('utf-8')
    plt.close()

    return jsonify({
        "status": "success",
        "description": "System Booking Statistics",
        "graph_type": "pie_chart",
        "image_base64": f"data:image/png;base64,{img_base64}"
    })

if __name__ == '__main__':
    # تشغيل الخادم على المنفذ 5000 المفتوح في الـ Docker
    app.run(host='0.0.0.0', port=5000)
