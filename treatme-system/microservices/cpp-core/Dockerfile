
FROM ubuntu:22.04
RUN apt-get update && apt-get install -y cmake g++ libboost-all-dev
# ملاحظة: في بيئة الإنتاج الفعلية، يجب تثبيت مكتبة SimpleAmqpClient و nlohmann-json هنا
COPY . /app
WORKDIR /app
RUN cmake . && make
CMD ["./TreatMeCppEngine"]
