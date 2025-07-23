#ifndef SEARCHSERVER_H
#define SEARCHSERVER_H

#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include "InvertedIndex.h"

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
    SearchServer(InvertedIndex& idx) : _index(idx) {};
    std::vector<std::vector<RelativeIndex>> search(const std::vector<std::string>& queries_input);

private:
    InvertedIndex& _index;
    std::vector<std::string> splitIntoWords(const std::string& query);
    std::vector<size_t> findDocsWithAllWords(const std::vector<std::string>& words);
    std::vector<RelativeIndex> calculateRelevance(const std::vector<size_t>& doc_ids,
        const std::vector<std::string>& words);
};

#endif // SEARCHSERVER_H
