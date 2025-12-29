#pragma once
#include <string>
#include <vector>
#include "inverted_index.h"

class Ranker
{
public:
    Ranker(double k1 = 1.5, double b = 0.75);

    std::vector<int> rank(const InvertedIndex &index, const std::string &query, int k = 10) const;

private:
    double k1;
    double b;
};