#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <iomanip>
#include <numeric>

#include "inverted_index.h"
#include "ranking.h"
#include "utils.h"

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <query>" << std::endl;
        return 1;
    }

    std::string query = argv[1];
    for (int i = 2; i < argc; ++i) {
        query += " ";
        query += argv[i];
    }

    // 1. Setup (Not measured)
    std::string metadata_path = "data/processed_docs/metadata.csv";
    std::unordered_map<int, std::string> doc_urls;
    load_metadata(metadata_path, doc_urls);

    InvertedIndex index;
    for (auto &[doc_id, url] : doc_urls) {
        std::string path = "data/processed_docs/index/" + format_doc_id(doc_id) + ".txt";
        index.add_document(doc_id, read_file(path));
    }
    index.finalizeIndex();

    Ranker ranker(1.5, 0.75, "data/stopwords.txt");

    // 2. Benchmark Query Execution
    const int iterations = 1000;
    std::vector<double> timings;
    timings.reserve(iterations);

    // Warmup
    for (int i = 0; i < 10; ++i) {
        ranker.rank(index, query, 10);
    }

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        ranker.rank(index, query, 10);
    }
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> total_duration = end - start;
    double average_ms = total_duration.count() / iterations;

    std::cout << "Query: \"" << query << "\"" << std::endl;
    std::cout << "Iterations: " << iterations << std::endl;
    std::cout << "Total time: " << total_duration.count() << " ms" << std::endl;
    std::cout << "Average time per query: " << std::fixed << std::setprecision(4) << average_ms << " ms" << std::endl;

    return 0;
}
