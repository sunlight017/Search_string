#ifndef CONVERTERJSON_H
#define CONVERTERJSON_H

#include <vector>
#include <string>
#include <nlohmann/json.hpp>

class ConverterJSON {
public:
    ConverterJSON();
    std::vector<std::string> GetTextDocuments();
    int GetResponsesLimit();
    std::vector<std::string> GetRequests();
    void putAnswers(std::vector<std::vector<std::pair<int, float>>> answers);

private:
    nlohmann::json config;
    nlohmann::json requests;
    void validateConfig();
    std::string readFile(const std::string& path);
};

#endif // CONVERTERJSON_H