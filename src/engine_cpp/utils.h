#pragma once
#include <unordered_map>
#include <string>

void load_metadata(const std::string &metadata_path, std::unordered_map<int, std::string> &doc_urls);