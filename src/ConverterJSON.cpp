#include "ConverterJSON.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <sstream>
ConverterJSON::ConverterJSON() {
    try {
        if (!std::filesystem::exists("config.json")) {
            throw std::runtime_error("config.json is missing");
        }
        std::ifstream configFile("config.json");
        config = nlohmann::json::parse(configFile);
        if (std::filesystem::exists("requests.json")) {
            std::ifstream requestsFile("requests.json");
            requests = nlohmann::json::parse(requestsFile);
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

    auto& configSection = config["config"];
    if (!configSection.contains("name") ||
        !configSection.contains("version")) {
        throw std::runtime_error("config file has incorrect format");
    }

    if (configSection["version"] != "0.1") {
        throw std::runtime_error("config.json has incorrect file version");
    }

    if (!config.contains("files")) {
        throw std::runtime_error("config file missing 'files' section");
    }
}

std::vector<std::string> ConverterJSON::GetTextDocuments() {
    std::vector<std::string> documents;

    if (!config.contains("files")) {
        return documents;
    }

    for (const auto& filePath : config["files"]) {
        if (!std::filesystem::exists(filePath)) {
            std::cerr << "Warning: File not found - " << filePath << std::endl;
            continue;
        }
        try {
            documents.push_back(readFile(filePath));
        }
        catch (...) {
            std::cerr << "Error reading file: " << filePath << std::endl;
        }
    }

    return documents;
}

int ConverterJSON::GetResponsesLimit() {
    return config["config"].value("max_responses", 5);
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

void ConverterJSON::putAnswers(const std::vector<std::vector<std::pair<size_t, float>>>& answers) {
    nlohmann::json result;

    for (size_t i = 0; i < answers.size(); i++) {
        std::string requestId = std::string("request") +
            (i < 9 ? std::string("00") : (i < 99 ? std::string("0") : std::string(""))) +
            std::to_string(i + 1);

        nlohmann::json requestResult;

        if (answers[i].empty()) {
            requestResult["result"] = false;
        }
        else {
            requestResult["result"] = true;

            if (answers[i].size() > 1) {
                nlohmann::json relevance;
                for (const auto& [doc_id, rank] : answers[i]) {
                    relevance.push_back({
                        {"docid", doc_id},
                        {"rank", rank}
                        });
                }
                requestResult["relevance"] = relevance;
            }
            else {
                requestResult["docid"] = answers[i][0].first;
                requestResult["rank"] = answers[i][0].second;
            }
        }

        result["answers"][requestId] = requestResult;
    }

    std::ofstream("answers.json") << result.dump(4);
}

std::string ConverterJSON::readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        throw std::runtime_error("Cannot open file: " + path);
    }
    return std::string(std::istreambuf_iterator<char>(file), {});
}
