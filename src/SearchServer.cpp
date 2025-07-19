#include "SearchServer.h"

std::vector<std::vector<RelativeIndex>> SearchServer::search(const std::vector<std::string>& queries_input) {
    std::vector<std::vector<RelativeIndex>> results;

    for (const auto& query : queries_input) {
        auto words = splitIntoWords(query);
        if (words.empty()) {
            results.emplace_back();
            continue;
        }

        std::sort(words.begin(), words.end(), [this](const std::string& a, const std::string& b) {
            return _index.GetWordCount(a).size() < _index.GetWordCount(b).size();
            });

        auto doc_ids = findDocsWithAllWords(words);
        auto relevance = calculateRelevance(doc_ids, words);

        results.push_back(relevance);
    }

    return results;
}

std::vector<std::string> SearchServer::splitIntoWords(const std::string& query) {
    std::vector<std::string> words;
    std::istringstream iss(query);
    std::string word;

    while (iss >> word) {
        word.erase(std::remove_if(word.begin(), word.end(),
            [](char c) { return !isalpha(c); }), word.end());
        std::transform(word.begin(), word.end(), word.begin(), ::tolower);

        if (!word.empty()) {
            words.push_back(word);
        }
    }

    std::sort(words.begin(), words.end());
    words.erase(std::unique(words.begin(), words.end()), words.end());

    return words;
}

std::vector<size_t> SearchServer::findDocsWithAllWords(const std::vector<std::string>& words) {
    if (words.empty()) return {};
    auto firstWordEntries = _index.GetWordCount(words[0]);
    std::vector<size_t> doc_ids;
    for (const auto& entry : firstWordEntries) {
        doc_ids.push_back(entry.doc_id);
    }
    for (size_t i = 1; i < words.size() && !doc_ids.empty(); ++i) {
        auto currentEntries = _index.GetWordCount(words[i]);
        std::vector<size_t> currentDocIds;
        for (const auto& entry : currentEntries) {
            currentDocIds.push_back(entry.doc_id);
        }

        std::vector<size_t> intersection;
        std::set_intersection(doc_ids.begin(), doc_ids.end(),
            currentDocIds.begin(), currentDocIds.end(),
            std::back_inserter(intersection));
        doc_ids = std::move(intersection);
    }

    return doc_ids;
}

std::vector<RelativeIndex> SearchServer::calculateRelevance(const std::vector<size_t>& doc_ids,
    const std::vector<std::string>& words) {
    std::vector<RelativeIndex> result;
    if (doc_ids.empty()) return result;
    std::vector<float> abs_relevance(doc_ids.size(), 0.0f);
    for (size_t i = 0; i < doc_ids.size(); ++i) {
        for (const auto& word : words) {
            auto entries = _index.GetWordCount(word);
            for (const auto& entry : entries) {
                if (entry.doc_id == doc_ids[i]) {
                    abs_relevance[i] += entry.count;
                    break;
                }
            }
        }
    }
    float max_relevance = *std::max_element(abs_relevance.begin(), abs_relevance.end());
    if (max_relevance == 0) return result;
    for (size_t i = 0; i < doc_ids.size(); ++i) {
        result.push_back({ doc_ids[i], abs_relevance[i] / max_relevance });
    }

    std::sort(result.begin(), result.end(), std::greater<>());

    return result;
}