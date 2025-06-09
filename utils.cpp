#include "utils.h"
#include <fstream>
#include <algorithm>
#include <ctime>
#include <sstream>
#include <iostream>

std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

void logChat(const std::string& userInput, const std::string& botResponse) {
    std::ofstream log("chat_log.txt", std::ios::app);
    time_t now = time(0);
    char* dt = ctime(&now);
    log << "[User] (" << dt << "): " << userInput << "\n";
    log << "[Bot] (" << dt << "): " << botResponse << "\n";
    log << "------------------------------------\n";
    log.close();
}

std::string getTime() {
    time_t now = time(0);
    char* dt = ctime(&now);
    return std::string(dt);
}

bool isMathQuery(const std::string& input) {
    return input.find('+') != std::string::npos ||
           input.find('-') != std::string::npos ||
           input.find('*') != std::string::npos ||
           input.find('/') != std::string::npos;
}

double evaluateExpression(const std::string& expr) {
    std::istringstream iss(expr);
    double total = 0, num;
    char op = '+';
    while (iss >> num) {
        switch (op) {
            case '+': total += num; break;
            case '-': total -= num; break;
            case '*': total *= num; break;
            case '/': if (num != 0) total /= num; else throw std::runtime_error("Divide by zero"); break;
        }
        iss >> op;
    }
    return total;
}
