#pragma once
#include <unordered_map>
#include <vector>
#include <string>

class InvertedIndex
{
public:
    void add_document(int doc_id, const std::string &text);
    
    // New TermID-based methods
    int get_term_id(const std::string &term) const;
    const std::vector<std::pair<int, int>> &get_postings(int term_id) const;
    double get_idf(int term_id) const;

    // String-based wrappers (for compatibility)
    const std::vector<std::pair<int, int>> &get_postings(const std::string &term) const;
    double get_idf(const std::string &term) const;

    int get_document_count() const;
    int get_document_length(int doc_id) const;
    double get_avg_document_length() const;
    void finalizeIndex();

private:
    std::unordered_map<std::string, int> vocabulary;
    int next_term_id = 0;
    std::unordered_map<int, std::unordered_map<int, int>> tempIndex;
    std::unordered_map<int, std::vector<std::pair<int, int>>> Index;
    void add_token(const std::string &token, int doc_id);
    std::unordered_map<int, int> doc_lengths;
    std::unordered_map<int, double> idf;
    void compute_idf();
    int total_docs = 0;
};