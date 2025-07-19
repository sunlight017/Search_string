#include "ConverterJSON.h"
#include <fstream>
#include <iostream>
#include <stdexcept>

ConverterJSON::ConverterJSON() {
    try {
        std::ifstream configFile("config.json");
        if (!configFile.is_open()) {
            throw std::runtime_error("config file is missing");
        }
        configFile >> config;
        configFile.close();

        std::ifstream requestsFile("requests.json");
        if (requestsFile.is_open()) {
            requestsFile >> requests;
            requestsFile.close();
        }

        validateConfig();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

void ConverterJSON::validateConfig() {
    if (config.empty() || !config.contains("config")) {
        throw std::runtime_error("config file is empty");
    }

    if (!config["config"].contains("name") ||
        !config["config"].contains("version")) {
        throw std::runtime_error("config file has incorrect format");
    }

    if (config["config"]["version"] != "0.1") {
        throw std::runtime_error("config.json has incorrect file version");
    }
}

std::vector<std::string> ConverterJSON::GetTextDocuments() {
    std::vector<std::string> documents;

    if (!config.contains("files")) {
        return documents;
    }

    for (const auto& filePath : config["files"]) {
        try {
            documents.push_back(readFile(filePath));
        }
        catch (const std::exception& e) {
            std::cerr << "Error reading file " << filePath << ": " << e.what() << std::endl;
        }
    }

    return documents;
}

int ConverterJSON::GetResponsesLimit() {
    if (config["config"].contains("max_responses")) {
        return config["config"]["max_responses"];
    }
    return 5;
}

std::vector<std::string> ConverterJSON::GetRequests() {
    std::vector<std::string> requestList;

    if (requests.empty() || !requests.contains("requests")) {
        return requestList;
    }

    for (const auto& request : requests["requests"]) {
        requestList.push_back(request);
    }

    return requestList;
}

void ConverterJSON::putAnswers(std::vector<std::vector<std::pair<int, float>>> answers) {
    nlohmann::json result;
    nlohmann::json answersJson;

    for (size_t i = 0; i < answers.size(); ++i) {
        std::string requestId = "request" + (i < 9 ? "00" : (i < 99 ? "0" : "")) + std::to_string(i + 1);
        nlohmann::json requestResult;

        if (answers[i].empty()) {
            requestResult["result"] = "false";
        }
        else {
            requestResult["result"] = "true";

            if (answers[i].size() > 1) {
                nlohmann::json relevanceArray;
                for (const auto& [docId, rank] : answers[i]) {
                    relevanceArray.push_back({
                        {"docid", docId},
                        {"rank", rank}
                        });
                }
                requestResult["relevance"] = relevanceArray;
            }
            else {
                requestResult["docid"] = answers[i][0].first;
                requestResult["rank"] = answers[i][0].second;
            }
        }

        answersJson[requestId] = requestResult;
    }

    result["answers"] = answersJson;

    std::ofstream outFile("answers.json");
    outFile << result.dump(4);
    outFile.close();
}

std::string ConverterJSON::readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("file not found");
    }

    std::string content((std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());
    file.close();

    return content;
}