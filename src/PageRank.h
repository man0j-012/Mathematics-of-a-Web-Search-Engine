#pragma once
#include <vector>
#include <unordered_map>

class PageRank {
public:
    // graph: adjacency list mapping doc -> list of outgoing doc ids
    static std::unordered_map<int,double> compute(const std::unordered_map<int, std::vector<int>>& graph,
                                                  int N, double alpha=0.85, int iters=40);
};
