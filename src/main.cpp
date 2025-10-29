#include "Tokenizer.h"
#include "InvertedIndex.h"
#include "DocumentStore.h"
#include "QueryEngine.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>

namespace fs = std::filesystem;

struct LoadedDoc { std::string title; std::string content; std::vector<std::string> links; };

static LoadedDoc parse_file(const std::string& path) {
    std::ifstream in(path);
    if (!in) throw std::runtime_error("Could not open file: "+path);
    std::string line;
    LoadedDoc d;
    while (std::getline(in, line)) {
        if (line.rfind("TITLE:", 0) == 0) {
            d.title = std::string(line.begin()+6, line.end());
            if (!d.title.empty() && d.title.front()==' ') d.title.erase(0,1);
        } else if (line.rfind("LINK:", 0) == 0) {
            std::string tgt(line.begin()+5, line.end());
            if (!tgt.empty() && tgt.front()==' ') tgt.erase(0,1);
            d.links.push_back(tgt);
        } else {
            d.content += line + "\n";
        }
    }
    return d;
}

int main(int argc, char** argv) {
    std::string dataDir = "data";
    std::string oneQuery;
    for (int i=1;i<argc;++i) {
        std::string a = argv[i];
        if (a=="--data" && i+1<argc) dataDir = argv[++i];
        else if (a=="--query" && i+1<argc) oneQuery = argv[++i];
        else if (a=="-h"||a=="--help") {
            std::cout << "Usage: tiny_search --data <folder> [--query \"terms\"]\n";
            return 0;
        }
    }

    // Load files
    std::map<std::string,int> filename_to_id;
    DocumentStore store;
    Tokenizer tok(false);
    InvertedIndex index;
    std::unordered_map<int,std::vector<int>> graph;

    // First pass: assign IDs
    for (auto& p : fs::directory_iterator(dataDir)) {
        if (!p.is_regular_file()) continue;
        std::string fname = p.path().filename().string();
        filename_to_id[fname] = (int)filename_to_id.size();
    }
    // Second pass: parse + index
    for (auto& p : fs::directory_iterator(dataDir)) {
        if (!p.is_regular_file()) continue;
        std::string fname = p.path().filename().string();
        auto ld = parse_file(p.path().string());
        auto toks = tok.tokenize(ld.title + "\n" + ld.content);
        std::vector<std::pair<std::string,int>> pairs;
        for (auto& t : toks) pairs.push_back({t.term, t.position});
        int doc_id = store.add(fname, ld.title, ld.content, (int)toks.size());
        index.add_document(doc_id, pairs);
        // Build link edges
        for (auto& l : ld.links) {
            auto it = filename_to_id.find(l);
            if (it != filename_to_id.end()) {
                graph[doc_id].push_back(it->second);
            }
        }
    }

    QueryEngine engine(store, index, graph);

    auto run_query = [&](const std::string& q){
        auto results = engine.search(q, 10);
        std::cout << "Results for: " << q << "\n";
        for (size_t i=0;i<results.size();++i) {
            const auto& r = results[i];
            std::cout << i+1 << ". [" << r.score << "] " << r.title << " (" << store.get(r.doc_id).filename << ")\n";
            std::cout << "   " << r.snippet << "\n";
        }
    };

    if (!oneQuery.empty()) {
        run_query(oneQuery);
        return 0;
    }
    std::cout << "TinySearch loaded " << store.size() << " docs from '"<<dataDir<<"'. Enter queries (empty to exit):\n";
    for (std::string q; std::cout << "> " && std::getline(std::cin, q); ) {
        if (q.empty()) break;
        run_query(q);
    }
    return 0;
}
