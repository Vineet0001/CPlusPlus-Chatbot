# 🤖 Smart C++ ChatBot

An advanced command-line AI chatbot built in C++ that combines local rule-based responses with real-time data from OpenWeather, Wikipedia, and OpenAI GPT.

[![Watch the demo](https://youtu.be/wqwePB8FMj4?si=dbMX7-fYOHhZjwlM)

> 🔗 [GitHub Repo](https://github.com/Vineet0001/CPlusPlus-Chatbot) &nbsp; | &nbsp; 🧠 [Live ChatBot Demo](https://youtu.be/wqwePB8FMj4?si=dbMX7-fYOHhZjwlM)

---

## ✨ Features

- ✅ Remembers your name
- 🌐 Fetches real-time weather (OpenWeatherMap)
- 📚 Wikipedia summaries
- 🧠 Fallback to OpenAI GPT when local responses fail
- ➗ Handles math expressions
- ⏰ Displays time/date
- 📄 Chat logging in text file
- 💡 JSON-based keyword-response mapping

---

## 🔧 Setup Instructions

### Requirements:
- g++ with C++17 support (via MSYS2 MinGW 64-bit)
- `libcurl` for API calls
- `json.hpp` (nlohmann JSON)
- `cacert.pem` for HTTPS support
- OpenWeather & OpenAI API keys

### 1. Clone the repo

git clone https://github.com/Vineet0001/CPlusPlus-Chatbot.git
cd CPlusPlus-Chatbot

### 2. Compile and run:
g++ -std=c++17 main.cpp chat_engine.cpp utils.cpp -lcurl -o chatbot
./chatbot

### 3. Example Commands:
my name is vineet
what is the weather in delhi
who is virat kohli
what is python
what is 3 * 7
what is time
bye


### 4. 📁 Project Structure
SimpleChatbot/
├── main.cpp
├── chat_engine.cpp
├── chat_engine.h
├── utils.cpp
├── utils.h
├── json.hpp
├── responses.json
├── secrets.h           # (excluded via .gitignore)
├── cacert.pem
├── chat_log.txt
├── README.md
└── .gitignore


### 5. 📜 License
MIT — Free to use, modify, and distribute with credit.



