#include "SearchServer.h"
#include <sstream>
std::vector<std::vector<RelativeIndex>> SearchServer::search(
    const std::vector<std::string>& queries_input)
{
    std::vector<std::vector<RelativeIndex>> results;

    for (const auto& query : queries_input) {
        auto words = splitIntoWords(query);
        if (query.empty()) {
            results.emplace_back();
            continue;
        }
        if (words.empty()) {
            results.emplace_back();
            continue;
        }

        auto doc_ids = findDocsWithAllWords(words);
        auto relevance = calculateRelevance(doc_ids, words);

        if (max_responses > 0 && relevance.size() > static_cast<size_t>(max_responses)) {
            relevance.resize(max_responses);
        }

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

        if (!word.empty() && word.length() <= 100) {
            words.push_back(word);
        }
    }

    return words;
}

std::vector<size_t> SearchServer::findDocsWithAllWords(const std::vector<std::string>& words) {
    if (words.empty()) return {};

    auto first_word_entries = index.GetWordCount(words[0]);
    std::vector<size_t> doc_ids;
    for (const auto& entry : first_word_entries) {
        doc_ids.push_back(entry.doc_id);
    }

    for (size_t i = 1; i < words.size() && !doc_ids.empty(); i++) {
        auto current_entries = index.GetWordCount(words[i]);
        std::vector<size_t> current_doc_ids;
        for (const auto& entry : current_entries) {
            current_doc_ids.push_back(entry.doc_id);
        }

        std::vector<size_t> intersection;
        std::set_intersection(
            doc_ids.begin(), doc_ids.end(),
            current_doc_ids.begin(), current_doc_ids.end(),
            std::back_inserter(intersection)
        );
        doc_ids = std::move(intersection);
    }

    return doc_ids;
}

std::vector<RelativeIndex> SearchServer::calculateRelevance(
    const std::vector<size_t>& doc_ids,
    const std::vector<std::string>& words)
{
    std::vector<RelativeIndex> result;
    if (doc_ids.empty()) return result;

    std::vector<float> abs_relevance(doc_ids.size(), 0.0f);
    float max_relevance = 0.0f;

    for (size_t i = 0; i < doc_ids.size(); i++) {
        for (const auto& word : words) {
            auto entries = index.GetWordCount(word);
            for (const auto& entry : entries) {
                if (entry.doc_id == doc_ids[i]) {
                    abs_relevance[i] += entry.count;
                    break;
                }
            }
        }
        max_relevance = std::max(max_relevance, abs_relevance[i]);
    }

    for (size_t i = 0; i < doc_ids.size(); i++) {
        if (max_relevance > 0) {
            result.push_back({ doc_ids[i], abs_relevance[i] / max_relevance });
        }
    }

    std::sort(result.begin(), result.end(), std::greater<>());

    return result;
}
