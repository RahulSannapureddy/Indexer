#pragma once
#include <unordered_map>
#include <vector>
#include <string>

class InvertedIndex
{
public:
    void add_document(int doc_id, const std::string &text);
    const std::vector<std::pair<int, int>> &get_postings(const std::string &term) const;

private:
    std::unordered_map<std::string, std::unordered_map<int, int>> tempIndex;
    std::unordered_map<std::string, std::vector<std::pair<int, int>>> Index;
    void finalizeIndex();
    void add_token(const std::string &token, int doc_id);
};