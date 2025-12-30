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

std::string format_doc_id(int doc_id)
{
    std::string doc_id_str = std::to_string(doc_id);

    int padding_length = 6 - doc_id_str.length();
    if (padding_length > 0)
    {
        doc_id_str = std::string(padding_length, '0') + doc_id_str;
    }

    return doc_id_str;
}

std::string read_file(const std::string &file_path)
{
    std::ifstream file(file_path, std::ios::binary);
    if (!file)
    {
        std::cerr << "Error opening file: " << file_path << std::endl;
        return "";
    }

    file.seekg(0, std::ios::end);
    const size_t file_size = static_cast<size_t>(file.tellg());
    file.seekg(0, std::ios::beg);

    std::string content(file_size, '\0');
    file.read(&content[0], file_size);

    return content;
}