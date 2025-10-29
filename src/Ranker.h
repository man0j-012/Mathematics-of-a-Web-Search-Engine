#pragma once
#include "InvertedIndex.h"
#include "DocumentStore.h"
#include <unordered_map>
#include <string>

struct Score {
    int doc_id;
    double score;
};

class Ranker {
public:
    Ranker(const InvertedIndex& idx, const DocumentStore& store, double k1=1.2, double b=0.75);
    std::unordered_map<int,double> bm25(const std::vector<std::string>& terms) const;
    static void normalize_inplace(std::unordered_map<int,double>& scores);
private:
    const InvertedIndex& idx_;
    const DocumentStore& store_;
    double k1_, b_;
};
