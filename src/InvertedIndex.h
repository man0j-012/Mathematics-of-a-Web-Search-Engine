#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include <algorithm>

struct Posting {
    int doc_id;
    std::vector<int> positions;
};

struct TermInfo {
    int df = 0;
    std::vector<Posting> postings;
};

class InvertedIndex {
public:
    void add_document(int doc_id, const std::vector<std::pair<std::string,int>>& terms);
    const TermInfo* get(const std::string& term) const;
    int doc_freq(const std::string& term) const;
    size_t vocabulary_size() const { return index_.size(); }
    const std::unordered_map<std::string, TermInfo>& raw() const { return index_; }
private:
    std::unordered_map<std::string, TermInfo> index_;
};
