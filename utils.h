#ifndef UTILS_H
#define UTILS_H

#include <string>

std::string toLower(const std::string& str);
void logChat(const std::string& userInput, const std::string& botResponse);
std::string getTime();
bool isMathQuery(const std::string& input);
double evaluateExpression(const std::string& expr);

#endif
