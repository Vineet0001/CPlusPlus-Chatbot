#include "chat_engine.h"
#include "utils.h"
#include "json.hpp"
#include "secrets.h"
#include <fstream>
#include <map>
#include <iostream>
#include <curl/curl.h>
#include <algorithm>

using json = nlohmann::json;

std::map<std::string, std::string> responses;
std::string userName = "";
const std::string weatherApiKey = "bc25195ef6adfd276d8ac87fc8929090";

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

void loadResponsesFromJSON(const std::string& filename) {
    std::ifstream file(filename);
    if (file.is_open()) {
        json j;
        file >> j;
        for (auto& [key, value] : j.items()) {
            responses[key] = value;
        }
    }
}

void setUserName(const std::string& name) {
    userName = name;
}

std::string getUserName() {
    return userName;
}

std::string getWeather(const std::string& city) {
    std::string url = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "&appid=" + weatherApiKey + "&units=metric";
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "Weather CURL error: " << curl_easy_strerror(res) << "\n";
        }
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();

    if (readBuffer.empty()) return "Could not fetch weather data.";

    try {
        auto j = json::parse(readBuffer);
        std::string description = j["weather"][0]["description"];
        float temp = j["main"]["temp"];
        return "It's " + std::to_string(temp) + "°C in " + city + " with " + description + ".";
    } catch (...) {
        return "Unable to parse weather data.";
    }
}

std::string getWikipediaSummary(const std::string& topic) {
    std::string url = "https://en.wikipedia.org/api/rest_v1/page/summary/" + topic;
    std::replace(url.begin(), url.end(), ' ', '_');

    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_CAINFO, "cacert.pem");
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Chatbot/1.0");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "Wikipedia CURL error: " << curl_easy_strerror(res) << "\n";
        }
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();

    if (readBuffer.empty()) {
        std::cerr << "Wikipedia API response was empty.\n";
        return "I couldn't fetch info from Wikipedia.";
    }

    try {
        auto j = json::parse(readBuffer);
        if (j.contains("extract"))
            return j["extract"];
        else
            return "Sorry, I couldn't find information on that.";
    } catch (...) {
        return "Failed to read Wikipedia response.";
    }
}

std::string getChatGPTResponse(const std::string& prompt) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    std::string jsonData = R"({
        "model": "gpt-3.5-turbo",
        "messages": [
            { "role": "system", "content": "You are a helpful assistant." },
            { "role": "user", "content": ")" + prompt + R"(" }
        ]
    })";

    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, ("Authorization: Bearer " + std::string(OPENAI_API_KEY)).c_str());

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.openai.com/v1/chat/completions");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_CAINFO, "cacert.pem");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "GPT CURL error: " << curl_easy_strerror(res) << "\n";
        }

        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    curl_slist_free_all(headers);

    if (readBuffer.empty()) {
        std::cerr << "GPT API response was empty.\n";
        return "GPT did not return a response.";
    }

    try {
        auto j = json::parse(readBuffer);
        return j["choices"][0]["message"]["content"];
    } catch (std::exception& e) {
        std::cerr << "GPT JSON parsing error: " << e.what() << "\n";
        std::cerr << "GPT raw response:\n" << readBuffer << "\n";
        return "Failed to parse GPT response.";
    }
}

void handleChat(const std::string& input) {
    std::string lowerInput = toLower(input);
    bool found = false;

    // Basic validation for incomplete weather question
    if (lowerInput.find("weather") != std::string::npos && lowerInput.find("in") == std::string::npos) {
        std::string response = "Please ask like: 'weather in Delhi' to get the forecast.";
        std::cout << "🤖 ChatBot: " << response << "\n";
        logChat(input, response);
        return;
    }

    // Math
    if (isMathQuery(lowerInput)) {
        try {
            double result = evaluateExpression(lowerInput);
            std::string response = "The answer is: " + std::to_string(result);
            std::cout << "🤖 ChatBot: " << response << "\n";
            logChat(input, response);
            return;
        } catch (...) {
            std::string response = "Sorry, I couldn't calculate that!";
            std::cout << "🤖 ChatBot: " << response << "\n";
            logChat(input, response);
            return;
        }
    }

    // Name input
    if (lowerInput.find("my name is") != std::string::npos) {
        size_t pos = lowerInput.find("my name is");
        std::string name = input.substr(pos + 11);
        name.erase(0, name.find_first_not_of(" "));
        setUserName(name);
        std::string response = "Nice to meet you, " + name + "!";
        std::cout << "🤖 ChatBot: " << response << "\n";
        logChat(input, response);
        return;
    }

    // Name recall
    if (lowerInput.find("what is my name") != std::string::npos ||
        lowerInput.find("who am i") != std::string::npos) {
        std::string response = userName.empty() ? "I don't know your name yet. Tell me!" : "You are " + userName + "!";
        std::cout << "🤖 ChatBot: " << response << "\n";
        logChat(input, response);
        return;
    }

    // Weather
    if (lowerInput.find("weather in") != std::string::npos) {
        std::string city = input.substr(lowerInput.find("weather in") + 10);
        city.erase(0, city.find_first_not_of(" "));
        std::string response = getWeather(city);
        std::cout << "🤖 ChatBot: " << response << "\n";
        logChat(input, response);
        return;
    }

    // Wikipedia
    if ((lowerInput.find("tell me about") != std::string::npos ||
         lowerInput.find("what is") != std::string::npos ||
         lowerInput.find("who is") != std::string::npos) &&
        lowerInput.find("weather") == std::string::npos &&
        lowerInput.find("time") == std::string::npos &&
        lowerInput.find("date") == std::string::npos) {

        std::string topic;
        if (lowerInput.find("tell me about") != std::string::npos)
            topic = input.substr(lowerInput.find("tell me about") + 14);
        else if (lowerInput.find("what is") != std::string::npos)
            topic = input.substr(lowerInput.find("what is") + 8);
        else
            topic = input.substr(lowerInput.find("who is") + 7);

        topic.erase(0, topic.find_first_not_of(" "));
        std::string response = getWikipediaSummary(topic);
        std::cout << "🤖 ChatBot: " << response << "\n";
        logChat(input, response);
        return;
    }

    // JSON keyword responses
    for (const auto& pair : responses) {
        if (lowerInput.find(pair.first) != std::string::npos) {
            std::string response = pair.second;
            if (response == "dynamic" && pair.first == "time") {
                response = getTime();
            }
            std::cout << "🤖 ChatBot: " << response << "\n";
            logChat(input, response);
            return;
        }
    }

    // GPT fallback
    std::string response = getChatGPTResponse(input);
    std::cout << "🤖 ChatBot: " << response << "\n";
    logChat(input, response);
}
