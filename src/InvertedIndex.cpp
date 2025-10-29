#include "InvertedIndex.h"

void InvertedIndex::add_document(int doc_id, const std::vector<std::pair<std::string,int>>& terms) {
    std::unordered_map<std::string, std::vector<int>> posmap;
    for (auto& kv : terms) posmap[kv.first].push_back(kv.second);
    for (auto& kv : posmap) {
        auto& info = index_[kv.first];
        info.df += 1;
        info.postings.push_back(Posting{doc_id, kv.second});
    }
}

const TermInfo* InvertedIndex::get(const std::string& term) const {
    auto it = index_.find(term);
    if (it==index_.end()) return nullptr;
    return &it->second;
}

int InvertedIndex::doc_freq(const std::string& term) const {
    auto it = index_.find(term);
    return (it==index_.end()?0:it->second.df);
}
