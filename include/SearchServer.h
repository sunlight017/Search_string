#ifndef SEARCHSERVER_H
#define SEARCHSERVER_H

#include "InvertedIndex.h"
#include <vector>
#include <string>
#include <algorithm>

struct RelativeIndex {
    size_t doc_id;
    float rank;

    bool operator ==(const RelativeIndex& other) const {
        return (doc_id == other.doc_id && rank == other.rank);
    }

    bool operator > (const RelativeIndex& other) const {
        return rank > other.rank;
    }
};

class SearchServer {
public:
    SearchServer(InvertedIndex& idx, int max_responses)
        : index(idx), max_responses(max_responses) {
    };

    std::vector<std::vector<RelativeIndex>> search(const std::vector<std::string>& queries_input);

private:
    InvertedIndex& index;
    const int max_responses;

    std::vector<std::string> splitIntoWords(const std::string& query);
    std::vector<size_t> findDocsWithAllWords(const std::vector<std::string>& words);
    std::vector<RelativeIndex> calculateRelevance(const std::vector<size_t>& doc_ids,
        const std::vector<std::string>& words);
};

#endif
