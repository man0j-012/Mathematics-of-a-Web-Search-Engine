#include "PageRank.h"
#include <unordered_set>

std::unordered_map<int,double> PageRank::compute(const std::unordered_map<int, std::vector<int>>& graph,
                                                 int N, double alpha, int iters) {
    std::unordered_map<int,double> pr;
    double init = 1.0 / (double)N;
    for (int i=0;i<N;++i) pr[i] = init;

    std::unordered_set<int> nodes;
    for (int i=0;i<N;++i) nodes.insert(i);

    for (int it=0; it<iters; ++it) {
        std::unordered_map<int,double> next;
        double dangling_sum = 0.0;
        for (int i: nodes) {
            auto itg = graph.find(i);
            if (itg==graph.end() || itg->second.empty()) {
                dangling_sum += pr[i];
            } else {
                double out = (double)itg->second.size();
                for (int j : itg->second) next[j] += pr[i] / out;
            }
        }
        double dangling_share = dangling_sum / (double)N;
        for (int i: nodes) {
            double s = next[i] + dangling_share;
            pr[i] = (1.0 - alpha) / (double)N + alpha * s;
        }
    }
    return pr;
}
