#include "inverted_index.h"
#include <sstream>
#include <algorithm>
#include <cmath>

void InvertedIndex::add_document(int doc_id, const std::string &text)
{
    std::istringstream iss(text);
    std::string token;
    int length = 0;

    while (iss >> token)
    {
        add_token(token, doc_id);
        length++;
    }

    doc_lengths[doc_id] = length;
    total_docs++;
}

const std::vector<std::pair<int, int>> &InvertedIndex::get_postings(int term_id) const
{
    static const std::vector<std::pair<int, int>> empty;
    auto it = Index.find(term_id);
    if (it != Index.end())
    {
        return it->second;
    }
    return empty;
}

const std::vector<std::pair<int, int>> &InvertedIndex::get_postings(const std::string &term) const
{
    int term_id = get_term_id(term);
    if (term_id != -1)
    {
        return get_postings(term_id);
    }
    static const std::vector<std::pair<int, int>> empty;
    return empty;
}

int InvertedIndex::get_term_id(const std::string &term) const
{
    auto it = vocabulary.find(term);
    if (it != vocabulary.end())
    {
        return it->second;
    }
    return -1;
}

int InvertedIndex::get_document_count() const
{
    return total_docs;
}

int InvertedIndex::get_document_length(int doc_id) const
{
    auto it = doc_lengths.find(doc_id);
    if (it != doc_lengths.end())
    {
        return it->second;
    }
    return 0; // if doc_id not found
}

double InvertedIndex::get_idf(int term_id) const
{
    auto it = idf.find(term_id);
    if (it != idf.end())
    {
        return it->second;
    }
    return 0.0; // if term not found
}

double InvertedIndex::get_idf(const std::string &term) const
{
    int term_id = get_term_id(term);
    if (term_id != -1)
    {
        return get_idf(term_id);
    }
    return 0.0;
}

double InvertedIndex::get_avg_document_length() const
{
    if (total_docs == 0)
        return 0.0;

    double total_length = 0.0;
    for (const auto &[doc_id, length] : doc_lengths)
    {
        total_length += length;
    }
    return total_length / total_docs;
}

void InvertedIndex::finalizeIndex()
{
    for (auto &[term_id, docMap] : tempIndex)
    {
        std::vector<std::pair<int, int>> postings;

        for (auto &[docID, freq] : docMap)
        {
            postings.emplace_back(docID, freq);
        }

        sort(postings.begin(), postings.end()); // sort by docID
        Index[term_id] = postings;
    }

    tempIndex.clear(); // Free up memory no longer needed
    compute_idf();
}

void InvertedIndex::add_token(const std::string &token, int doc_id)
{
    int term_id;
    auto it = vocabulary.find(token);
    if (it == vocabulary.end())
    {
        term_id = next_term_id++;
        vocabulary[token] = term_id;
    }
    else
    {
        term_id = it->second;
    }
    tempIndex[term_id][doc_id]++;
}

void InvertedIndex::compute_idf()
{
    int N = total_docs;
    for (const auto &[term_id, postings] : Index)
    {
        int df = postings.size();
        idf[term_id] = log((N - df + 0.5) / (df + 0.5));
    }
}