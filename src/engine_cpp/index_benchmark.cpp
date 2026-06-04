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
    // Load metadata
    std::string metadata_path = "data/processed_docs/metadata.csv";
    std::unordered_map<int, std::string> doc_urls;

    load_metadata(metadata_path, doc_urls);

    if (doc_urls.empty()) {
        std::cerr << "Error: No metadata loaded." << std::endl;
        return 1;
    }

    // Build inverted index
    InvertedIndex index;

    for (auto &[doc_id, url] : doc_urls)
    {
        std::string path = "data/processed_docs/index/" + format_doc_id(doc_id) + ".txt";
        std::string text = read_file(path);
        if (!text.empty()) {
            index.add_document(doc_id, text);
        }
    }

    index.finalizeIndex();
    
    // We exit here to measure only the indexing time
    return 0;
}
