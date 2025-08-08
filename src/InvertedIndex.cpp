#include "InvertedIndex.h"
#include <sstream>
#include <cctype>

void InvertedIndex::UpdateDocumentBase(std::vector<std::string> input_docs) {
    std::lock_guard<std::mutex> lock(docs_mutex);
    docs = std::move(input_docs);
    freq_dictionary.clear();

    std::vector<std::thread> threads;
    for (size_t doc_id = 0; doc_id < docs.size(); doc_id++) {
        threads.emplace_back(&InvertedIndex::indexDocument, this, doc_id, docs[doc_id]);
    }

    for (auto& thread : threads) {
        thread.join();
    }
}

void InvertedIndex::indexDocument(size_t doc_id, const std::string& text) {
    auto words = splitIntoWords(text);
    std::map<std::string, size_t> wordCounts;

    for (const auto& word : words) {
        wordCounts[word]++;
    }

    std::lock_guard<std::mutex> lock(docs_mutex);
    for (const auto& [word, count] : wordCounts) {
        freq_dictionary[word].push_back({ doc_id, count });
    }
}

std::vector<Entry> InvertedIndex::GetWordCount(const std::string& word) {
    std::lock_guard<std::mutex> lock(docs_mutex);
    auto it = freq_dictionary.find(word);
    return it != freq_dictionary.end() ? it->second : std::vector<Entry>{};
}

std::vector<std::string> InvertedIndex::splitIntoWords(const std::string& text) {
    std::vector<std::string> words;
    std::istringstream iss(text);
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
