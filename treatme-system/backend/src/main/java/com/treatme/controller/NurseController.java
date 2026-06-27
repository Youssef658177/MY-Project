
package com.treatme.controller;

import com.treatme.dto.NurseDTO;
import com.treatme.service.NurseService;
import org.springframework.web.bind.annotation.*;
import java.util.List;

@RestController
@RequestMapping("/api/nurses")
public class NurseController {
    private final NurseService service;

    public NurseController(NurseService service) {
        this.service = service;
    }

    @GetMapping
    public List<NurseDTO> getNurses() {
        return service.getAllNurses();
    }

    @PostMapping("/optimize")
    public String optimize() {
        service.optimizeInCpp();
        return "Task sent to C++ Engine via RabbitMQ!";
    }
}
