#include "inverted_index.h"
#include <sstream>
#include <algorithm>
using namespace std;

void InvertedIndex::add_document(int doc_id, const std::string &text)
{
    istringstream iss(text);
    string token;
    int length = 0;

    while (iss >> token)
    {
        add_token(token, doc_id);
        length++;
    }

    doc_lengths[doc_id] = length;
    total_docs++;
}

const std::vector<std::pair<int, int>> &InvertedIndex::get_postings(const std::string &term) const
{
    static const vector<pair<int, int>> empty; // static empty vector to return if term not found
    auto it = Index.find(term);
    if (it != Index.end())
    {
        return it->second;
    }
    return empty;
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

void InvertedIndex::finalizeIndex()
{
    for (auto &[token, docMap] : tempIndex)
    {
        vector<pair<int, int>> postings;

        for (auto &[docID, freq] : docMap)
        {
            postings.emplace_back(docID, freq);
        }

        sort(postings.begin(), postings.end()); // sort by docID
        Index[token] = postings;
    }
}

void InvertedIndex::add_token(const std::string &token, int doc_id)
{
    tempIndex[token][doc_id]++;
}
