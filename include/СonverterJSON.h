#ifndef CONVERTERJSON_H
#define CONVERTERJSON_H

#include <nlohmann/json.hpp>
#include <vector>
#include <string>
#include <filesystem>

class ConverterJSON {
public:
    ConverterJSON();

    std::vector<std::string> GetTextDocuments();
    int GetResponsesLimit();
    std::vector<std::string> GetRequests();
    void putAnswers(const std::vector<std::vector<std::pair<size_t, float>>>& answers);

private:
    nlohmann::json config;
    nlohmann::json requests;

    void validateConfig();
    std::string readFile(const std::string& path);
};

#endif
