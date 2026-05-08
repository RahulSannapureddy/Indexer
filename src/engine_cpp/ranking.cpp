#include "ranking.h"
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

Ranker::Ranker(double k1_, double b_, const std::string &stopwords_file) : k1(k1_), b(b_)
{
    load_stopwords(stopwords_file);
}

std::vector<int> Ranker::rank(const InvertedIndex &index, const std::string &query, int k) const
{
    std::unordered_map<int, double> scores;

    double avg_doc_length = index.get_avg_document_length();
    if (avg_doc_length == 0.0)
        return {}; // avoid division by zero

    std::string_view sv(query);
    size_t start = 0;
    
    std::string term;
    term.reserve(32); // Pre-allocate to minimize reallocations

    while (start < sv.size())
    {
        // Skip punctuation and whitespace (acting as delimiters)
        while (start < sv.size() && (std::isspace(static_cast<unsigned char>(sv[start])) ||
                                     std::ispunct(static_cast<unsigned char>(sv[start]))))
        {
            start++;
        }
        if (start >= sv.size())
            break;

        size_t end = start;
        while (end < sv.size() && !std::isspace(static_cast<unsigned char>(sv[end])) &&
               !std::ispunct(static_cast<unsigned char>(sv[end])))
        {
            end++;
        }

        std::string_view token = sv.substr(start, end - start);
        start = end;

        // Lowercase the token into our reusable string
        term.clear();
        for (char c : token)
        {
            term += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        }

        if (stopwords.find(term) == stopwords.end())
        {
            double idf = index.get_idf(term);
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
    }

    std::vector<std::pair<int, double>> scored_docs(scores.begin(), scores.end());
    std::sort(scored_docs.begin(), scored_docs.end(), [](const auto &a, const auto &b)
              { return a.second > b.second; });

    std::vector<int> results;
    for (int i = 0; i < k && i < (int)scored_docs.size(); i++)
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
