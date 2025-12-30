#pragma once
#include <string>
#include <vector>
#include <unordered_set>
#include "inverted_index.h"

class Ranker
{
public:
    Ranker(double k1 = 1.5, double b = 0.75, const std::string &stopwords_file = "data/stopwords.txt");

    std::vector<int> rank(const InvertedIndex &index, const std::string &query, int k = 10) const;

private:
    double k1;
    double b;

    std::unordered_set<std::string> stopwords;

    void load_stopwords(const std::string &stopwords_file);
    std::string normalize_query(const std::string &query) const;
};