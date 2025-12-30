#pragma once
#include <unordered_map>
#include <string>

void load_metadata(const std::string &metadata_path, std::unordered_map<int, std::string> &doc_urls);
std::string format_doc_id(int doc_id);
std::string read_file(const std::string &file_path);