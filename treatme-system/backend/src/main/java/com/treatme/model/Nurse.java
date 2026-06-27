package com.treatme.model;

import jakarta.persistence.*;

@Entity
@Table(name = "Nurses")
public class Nurse {
    @Id @GeneratedValue(strategy = GenerationType.IDENTITY)
    private int id;
    private String name;
    private String specialty;
    private int capacity;
    private double rating;
    private int deadlineHours;
    
    @Version // للـ Optimistic Locking
    private int version;

    // Getters and Setters
    public int getId() { return id; }
    public String getName() { return name; }
    public String getSpecialty() { return specialty; }
    public double getRating() { return rating; }
}
