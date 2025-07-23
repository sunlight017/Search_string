#include <iostream>
#include "InvertedIndex.h"
#include "SearchServer.h"
#include "СonverterJSON.h"

int main() {
    try {
        ConverterJSON converter;
        InvertedIndex index;

        std::cout << "Starting " << converter.GetResponsesLimit() << std::endl;
        auto documents = converter.GetTextDocuments();
        index.UpdateDocumentBase(documents);
        SearchServer server(index);
        auto requests = converter.GetRequests();
        auto searchResults = server.search(requests);
        std::vector<std::vector<std::pair<int, float>>> answers;
        for (const auto& result : searchResults) {
            std::vector<std::pair<int, float>> answer;
            for (const auto& relIndex : result) {
                answer.emplace_back(relIndex.doc_id, relIndex.rank);
            }
            answers.push_back(answer);
        }

        converter.putAnswers(answers);
        std::cout << "Search complete. Results saved to answers.json" << std::endl;

    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
