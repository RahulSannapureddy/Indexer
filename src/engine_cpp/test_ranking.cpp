#include <iostream>
#include <cassert>
#include <vector>
#include "inverted_index.h"
#include "ranking.h"

void test_ranking() {
    InvertedIndex index;
    index.add_document(1, "the quick brown fox");
    index.add_document(2, "jumped over the lazy dog");
    index.add_document(3, "the brown dog");
    index.finalizeIndex();

    Ranker ranker(1.5, 0.75, "data/stopwords.txt");

    // "the" is likely a stopword, "brown" is in 1 and 3.
    std::vector<int> results = ranker.rank(index, "brown dog", 10);
    
    std::cout << "Results for 'brown dog': ";
    for (int id : results) std::cout << id << " ";
    std::cout << std::endl;

    // Current baseline behavior: 2 1 3
    assert(!results.empty());
    assert(results[0] == 2); 
}

int main() {
    try {
        test_ranking();
        std::cout << "Test passed!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
