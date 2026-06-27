
CREATE DATABASE IF NOT EXISTS TreatMe_DB;
USE TreatMe_DB;

CREATE TABLE Nurses (
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    specialty VARCHAR(100) NOT NULL,
    capacity INT NOT NULL,
    rating DOUBLE NOT NULL,
    deadlineHours INT NOT NULL,
    version INT DEFAULT 0
);

INSERT INTO Nurses (name, specialty, capacity, rating, deadlineHours) VALUES 
('سارة', 'طوارئ', 2, 4.9, 3), ('محمد', 'كبار السن', 5, 4.7, 5);
