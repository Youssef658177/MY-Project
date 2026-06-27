
package com.treatme;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.cache.annotation.EnableCaching;

@SpringBootApplication
@EnableCaching
public class TreatMeApplication {
    public static void main(String[] args) {
        SpringApplication.run(TreatMeApplication.class, args);
    }
}
