
package com.treatme.config;

import org.springframework.amqp.core.Queue;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

@Configuration
public class RabbitMQConfig {
    @Bean
    public Queue optimizationQueue() {
        return new Queue("cpp_optimization_queue", true);
    }
}
