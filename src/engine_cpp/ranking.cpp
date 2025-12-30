#include "ranking.h"
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <algorithm>

Ranker::Ranker(double k1_, double b_, const std::string &stopwords_file) : k1(k1_), b(b_)
{
    load_stopwords(stopwords_file);
}

std::vector<int> Ranker::rank(const InvertedIndex &index, const std::string &query, int k) const
{
    std::unordered_map<int, double> scores;

    std::string normalized_query = normalize_query(query);
    if (normalized_query.empty())
        return {};

    std::istringstream iss(normalized_query);
    std::string term;

    double avg_doc_length = index.get_avg_document_length();
    if (avg_doc_length == 0.0)
        return {}; // avoid division by zero

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
            double score = idf * (tf * (k1 + 1)) / (tf + k1 * (1 - b + b * (doc_length / avg_doc_length)));
            scores[doc_id] += score;
        }
    }

    std::vector<std::pair<int, double>> scored_docs(scores.begin(), scores.end());
    std::sort(scored_docs.begin(), scored_docs.end(), [](const auto &a, const auto &b)
              { return a.second > b.second; });

    std::vector<int> results;
    for (int i = 0; i < k && i < scored_docs.size(); i++)
    {
        results.push_back(scored_docs[i].first);
    }

    return results;
}

void Ranker::load_stopwords(const std::string &stopwords_file)
{
    std::ifstream file(stopwords_file);
    if (!file.is_open())
        throw std::runtime_error("Could not open stopwords file: " + stopwords_file);

    std::string word;

    while (file >> word)
    {
        stopwords.insert(word);
    }
}

std::string Ranker::normalize_query(const std::string &query) const
{
    std::string cleaned;
    cleaned.reserve(query.size());
    for (char c : query)
    {
        if (!std::ispunct(static_cast<unsigned char>(c)))
            cleaned += std::tolower(static_cast<unsigned char>(c));
        else
            cleaned += ' ';
    }

    std::istringstream iss(cleaned);
    std::string term;
    std::string normalized = "";

    while (iss >> term)
    {
        if (stopwords.find(term) == stopwords.end())
        {
            if (!normalized.empty())
                normalized += " ";
            normalized += term;
        }
    }

    return normalized;
}