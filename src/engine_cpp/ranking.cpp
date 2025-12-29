#include "ranking.h"
#include <unordered_map>
#include <sstream>
#include <algorithm>
#include <cmath>
using namespace std;

Ranker::Ranker(double k1_, double b_) : k1(k1_), b(b_) {}

std::vector<int> Ranker::rank(const InvertedIndex &index, const std::string &query, int k) const
{
    unordered_map<int, double> scores;

    istringstream iss(query);
    string term;

    double avg_doc_length = index.get_avg_document_length();
    int N = index.get_document_count();

    while (iss >> term)
    {
        double idf = index.get_idf(term);
        if (idf == 0.0)
            continue; // skip terms not in index

        const auto &postings = index.get_postings(term);

        for (const auto &posting : postings)
        {
            int doc_id = posting.first;
            int tf = posting.second;
            int doc_length = index.get_document_length(doc_id);
            double score = idf * (tf * (k1 + 1)) / (tf + k1) * (doc_length / avg_doc_length);
            scores[doc_id] += score;
        }
    }

    vector<pair<int, double>> scored_docs(scores.begin(), scores.end());
    sort(scored_docs.begin(), scored_docs.end(), [](const auto &a, const auto &b)
         { return a.second > b.second; });

    vector<int> results;
    for (int i = 0; i < k && i < scored_docs.size(); i++)
    {
        results.push_back(scored_docs[i].first);
    }

    return results;
}