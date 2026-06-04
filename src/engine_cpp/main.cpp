#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <iomanip>

#include "inverted_index.h"
#include "ranking.h"
#include "utils.h"

int main()
{
    auto start_load = std::chrono::high_resolution_clock::now();
    
    // Load metadata
    std::string metadata_path = "data/processed_docs/metadata.csv";
    std::unordered_map<int, std::string> doc_urls;

    load_metadata(metadata_path, doc_urls);

    // Build inverted index
    InvertedIndex index;

    for (auto &[doc_id, url] : doc_urls)
    {
        std::string path = "data/processed_docs/index/" + format_doc_id(doc_id) + ".txt";

        std::string text = read_file(path);
        index.add_document(doc_id, text);
    }

    index.finalizeIndex();
    
    auto end_load = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> load_duration = end_load - start_load;
    
    // Signal to UI that loading is complete and report benchmark
    std::cout << "LOAD_TIME_MS|" << std::fixed << std::setprecision(3) << load_duration.count() << std::endl;

    // create ranking object
    Ranker ranker(1.5, 0.75, "data/stopwords.txt");

    // UI loop
    std::string query;
    while (std::getline(std::cin, query))
    {
        if (query == "exit" || query.empty())
            break;

        auto start = std::chrono::high_resolution_clock::now();
        std::vector<int> top_docs = ranker.rank(index, query, 10);
        auto end = std::chrono::high_resolution_clock::now();
        
        std::chrono::duration<double, std::milli> duration = end - start;

        for (int doc_id : top_docs)
        {
            // Print each result on its own line
            std::cout << doc_id << "|" << doc_urls[doc_id] << "\n";
        }
        std::cout << "TIME_MS|" << std::fixed << std::setprecision(3) << duration.count() << "\n";
        std::cout << "END_OF_RESULTS" << std::endl;
    }

    return 0;
}