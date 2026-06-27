# 🏥 TreatMe - Enterprise Home Healthcare System

![Architecture](https://img.shields.io/badge/Architecture-Microservices-blue)
![Java](https://img.shields.io/badge/Java-Spring_Boot-green)
![C++](https://img.shields.io/badge/C++-17-blue)
![Python](https://img.shields.io/badge/Python-3.10-yellow)
![Docker](https://img.shields.io/badge/Docker-Zero_Trust_Network-2496ED)

TreatMe is a highly scalable, event-driven microservices platform designed to manage home healthcare and nursing bookings. Built with rigorous software engineering standards, it ensures high availability, robust data consistency, and absolute type safety.

## 🏗️ System Architecture

The project adopts a Monorepo structure, integrating four distinct services communicating over an isolated Zero-Trust Docker network:

1. **Frontend (Nginx/HTML/JS):** Lightweight, responsive UI serving as the entry point.
2. **Java Core (Spring Boot):** The central orchestrator handling authentication, REST APIs, and database transactions.
3. **C++ Engine:** A high-performance event consumer listening to RabbitMQ for scheduling optimizations.
4. **Python Analytics:** An internal ML/Graphing service isolated via Resilience4j Circuit Breakers.

### 🛡️ Infrastructure & Middleware
* **Database:** MySQL 8.0 with Optimistic Locking (`@Version`) to prevent booking race conditions.
* **Caching:** Redis layer to reduce disk I/O and database load.
* **Message Broker:** RabbitMQ enabling asynchronous, non-blocking communication between Java and C++.
* **Observability:** Zipkin integration for distributed tracing across all microservices.

## ✨ Key Engineering Features

* **Strict Type Safety:** Comprehensive use of `Enums` (e.g., `BookingStatus`) to enforce data integrity at the application layer.
* **Centralized Error Handling:** Implementation of `@ControllerAdvice` and custom `DatabaseException` for graceful error resolution and clear API responses.
* **Decoupled Processing:** Leveraging C++ memory management and speed for background computational tasks via AMQP.
* **Network Isolation:** Services are segregated into specific bridge networks (`frontend_net`, `backend_net`, `db_net`, `broker_net`), ensuring containers only communicate with necessary components.

## 🚀 Getting Started

### Prerequisites
* Docker & Docker Compose
* Git

### Installation & Execution

1. Clone the repository:
   ```bash
   git clone [https://github.com/your-username/treatme-system.git](https://github.com/your-username/treatme-system.git)
   cd treatme-system
