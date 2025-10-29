#pragma once
#include "Tokenizer.h"
#include "InvertedIndex.h"
#include "DocumentStore.h"
#include "Ranker.h"
#include "PageRank.h"
#include <unordered_map>
#include <string>
#include <vector>

struct SearchResult {
    int doc_id;
    double score;
    std::string title;
    std::string snippet;
};

class QueryEngine {
public:
    QueryEngine(const DocumentStore& store, const InvertedIndex& idx,
                const std::unordered_map<int,std::vector<int>>& link_graph);
    std::vector<SearchResult> search(const std::string& query, size_t k=10);
private:
    const DocumentStore& store_;
    const InvertedIndex& idx_;
    Tokenizer tok_;
    Ranker ranker_;
    std::unordered_map<int,double> pr_;
    static bool phrase_match(const std::vector<int>& posVec, const std::vector<int>& nextPos);
    static std::string make_snippet(const std::string& content, const std::vector<std::string>& terms, size_t maxlen=180);
};
