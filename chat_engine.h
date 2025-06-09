#ifndef CHAT_ENGINE_H
#define CHAT_ENGINE_H

#include <string>

void loadResponsesFromJSON(const std::string& filename);
void handleChat(const std::string& input);
void setUserName(const std::string& name);
std::string getUserName();
std::string getWeather(const std::string& city);
std::string getWikipediaSummary(const std::string& topic);
std::string getChatGPTResponse(const std::string& prompt);

#endif
