#include <iostream>
#include <string>
#include "chat_engine.h"
#include "utils.h"

int main() {
    loadResponsesFromJSON("responses.json");

    std::string input;
    std::cout << "🤖 ChatBot: Hello! Type 'bye' to exit.\n";

    while (true) {
        std::cout << "\nYou: ";
        std::getline(std::cin, input);

        if (toLower(input) == "bye") {
            handleChat("bye");
            break;
        }

        handleChat(input);
    }

    return 0;
}
