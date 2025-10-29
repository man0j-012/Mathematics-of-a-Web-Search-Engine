#include "Ranker.h"
#include <cmath>
#include <algorithm>

Ranker::Ranker(const InvertedIndex& idx, const DocumentStore& store, double k1, double b)
    : idx_(idx), store_(store), k1_(k1), b_(b) {}

std::unordered_map<int,double> Ranker::bm25(const std::vector<std::string>& terms) const {
    std::unordered_map<int,double> scores;
    double avgdl = store_.avg_doc_len();
    int N = store_.size();
    for (const auto& t : terms) {
        const TermInfo* info = idx_.get(t);
        if (!info) continue;
        int df = info->df;
        double idf = std::log(( (N - df + 0.5) / (df + 0.5 + 1e-9) ));
        for (const auto& p : info->postings) {
            int dl = store_.get(p.doc_id).length_tokens;
            double tf = (double)p.positions.size();
            double denom = tf + k1_ * (1.0 - b_ + b_ * (dl / (avgdl + 1e-9)));
            double s = idf * ((tf * (k1_ + 1.0)) / (denom + 1e-9));
            scores[p.doc_id] += s;
        }
    }
    return scores;
}

void Ranker::normalize_inplace(std::unordered_map<int,double>& scores) {
    double maxv = 0.0;
    for (auto& kv : scores) maxv = std::max(maxv, kv.second);
    if (maxv <= 0.0) return;
    for (auto& kv : scores) kv.second /= maxv;
}
