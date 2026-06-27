
package com.treatme.service;

import com.treatme.model.Nurse;
import com.treatme.dto.NurseDTO;
import com.treatme.repository.NurseRepository;
import org.springframework.amqp.rabbit.core.RabbitTemplate;
import org.springframework.cache.annotation.Cacheable;
import org.springframework.stereotype.Service;
import java.util.List;
import java.util.stream.Collectors;

@Service
public class NurseService {
    private final NurseRepository repository;
    private final RabbitTemplate rabbitTemplate;

    public NurseService(NurseRepository repository, RabbitTemplate rabbitTemplate) {
        this.repository = repository;
        this.rabbitTemplate = rabbitTemplate;
    }

    @Cacheable(value = "nursesData")
    public List<NurseDTO> getAllNurses() {
        return repository.findAll().stream()
                .map(n -> new NurseDTO(n.getId(), n.getName(), n.getSpecialty(), n.getRating()))
                .collect(Collectors.toList());
    }

    public void optimizeInCpp() {
        List<Nurse> nurses = repository.findAll();
        rabbitTemplate.convertAndSend("cpp_optimization_queue", nurses.toString()); // إرسال مبسط
    }
}
