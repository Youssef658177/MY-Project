
package com.treatme.exception;

import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.ControllerAdvice;
import org.springframework.web.bind.annotation.ExceptionHandler;

@ControllerAdvice
public class GlobalExceptionHandler {

    // اصطياد أخطاء قاعدة البيانات (مثل التزامن والـ Optimistic Locking)
    @ExceptionHandler(DatabaseException.class)
    public ResponseEntity<String> handleDatabaseException(DatabaseException ex) {
        return new ResponseEntity<>(ex.getMessage(), HttpStatus.CONFLICT);
    }

    // اصطياد أي خطأ عام غير متوقع في النظام
    @ExceptionHandler(Exception.class)
    public ResponseEntity<String> handleGeneralException(Exception ex) {
        return new ResponseEntity<>("حدث خطأ في الخادم الداخلي: " + ex.getMessage(), HttpStatus.INTERNAL_SERVER_ERROR);
    }
}
