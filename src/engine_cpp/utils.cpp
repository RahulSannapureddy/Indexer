#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "utils.h"

void load_metadata(const std::string &metadata_path, std::unordered_map<int, std::string> &doc_urls)
{
    std::ifstream file(metadata_path);
    std::string line;
    std::getline(file, line); // Skip header line

    while (std::getline(file, line))
    {
        std::istringstream ss(line);
        std::string doc_id_str, filename, url;

        std::getline(ss, doc_id_str, ',');
        std::getline(ss, filename, ',');
        std::getline(ss, url, ',');

        int doc_id = std::stoi(doc_id_str);

        doc_urls[doc_id] = url;
    }
}