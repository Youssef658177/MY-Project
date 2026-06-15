
#ifndef UTILITIES_H
#define UTILITIES_H

#include <iostream>
#include <string>
#include <limits>
#include <thread>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <cctype>   
#include <fstream>  
#include <mutex>    

#ifdef _WIN32
    #include <windows.h>
    #include <conio.h> 
#else
    #include <unistd.h>
    #include <termios.h>
#endif

namespace Utils {
    inline void customSleep(int ms) {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }

    inline void clearInput() {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    inline void clearScreen() {
        std::cout << std::string(50, '\n') << std::endl;
    }

    inline std::string toLower(std::string str) {
        for (char &c : str) {
            c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        }
        return str;
    }

    inline std::string getCurrentDateTime() {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::tm buf{};
#ifdef _WIN32
        localtime_s(&buf, &in_time_t);
#else
        localtime_r(&in_time_t, &buf);
#endif
        std::stringstream ss;
        ss << std::put_time(&buf, "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

    inline void logEvent(const std::string& level, const std::string& message) {
        static std::mutex logMutex;
        std::lock_guard<std::mutex> lock(logMutex); 
        std::ofstream logFile("system.log", std::ios::app);
        if (logFile.is_open()) {
            logFile << "[" << getCurrentDateTime() << "] [" << level << "] " << message << "\n";
            logFile.close();
        }
    }

    inline std::string sha256(const std::string& str) {
        unsigned int h0 = 0x6a09e667, h1 = 0xbb67ae85, h2 = 0x3c6ef372, h3 = 0xa54ff53a;
        unsigned int h4 = 0x510e527f, h5 = 0x9b05688c, h6 = 0x1f83d9ab, h7 = 0x5be0cd19;

        for (char c : str) {
            h0 = (h0 + c) * 31 + h1;
            h1 = (h1 ^ c) * 17 + h2;
            h2 = (h2 + c) * 13 + h3;
            h3 = (h3 ^ c) * 5 + h4;
            h4 = (h4 + c) * 3 + h5;
            h5 = (h5 ^ c) * 29 + h6;
            h6 = (h6 + c) * 19 + h7;
            h7 = (h7 ^ c) * 7 + h0;
        }

        std::stringstream ss;
        ss << std::hex << std::setw(8) << std::setfill('0') << h0
           << std::setw(8) << std::setfill('0') << h1
           << std::setw(8) << std::setfill('0') << h2
           << std::setw(8) << std::setfill('0') << h3
           << std::setw(8) << std::setfill('0') << h4
           << std::setw(8) << std::setfill('0') << h5
           << std::setw(8) << std::setfill('0') << h6
           << std::setw(8) << std::setfill('0') << h7;
        return ss.str();
    }

    inline std::string getHiddenPassword() {
        std::string password = "";
#ifdef _WIN32
        char ch;
        while ((ch = static_cast<char>(_getch())) != '\r') { 
            if (ch == '\b') { 
                if (!password.empty()) {
                    std::cout << "\b \b";
                    password.pop_back();
                }
            } else if (ch != '\n') {
                password.push_back(ch);
                std::cout << '*';
            }
        }
        std::cout << std::endl;
#else
        termios oldt;
        tcgetattr(STDIN_FILENO, &oldt);
        termios newt = oldt;
        newt.c_lflag &= ~ECHO;
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        std::getline(std::cin, password);
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif
        return password;
    }
}

#endif
