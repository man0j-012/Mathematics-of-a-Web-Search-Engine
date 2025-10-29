#include "QueryEngine.h"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <unordered_set>

QueryEngine::QueryEngine(const DocumentStore& store, const InvertedIndex& idx,
                         const std::unordered_map<int,std::vector<int>>& link_graph)
    : store_(store), idx_(idx), tok_(false), ranker_(idx, store) {
    pr_ = PageRank::compute(link_graph, store.size(), 0.85, 40);
}

static std::vector<std::string> split_query_terms(const std::string& q) {
    // Support quoted phrases and OR
    std::vector<std::string> tokens;
    bool in_quote=false;
    std::string cur;
    for (size_t i=0;i<=q.size();++i) {
        char ch = (i<q.size()? q[i] : ' ');
        if (ch=='"') { 
            if (in_quote) { tokens.push_back("\""+cur+"\""); cur.clear(); in_quote=false; }
            else { if (!cur.empty()) { tokens.push_back(cur); cur.clear(); } in_quote=true; }
        } else if (!in_quote && std::isspace((unsigned char)ch)) {
            if (!cur.empty()) { tokens.push_back(cur); cur.clear(); }
        } else {
            cur.push_back(ch);
        }
    }
    if (!cur.empty()) tokens.push_back(cur);
    return tokens;
}

bool QueryEngine::phrase_match(const std::vector<int>& a, const std::vector<int>& b) {
    // check if exists i in a and j in b such that b has position == i+1
    std::unordered_set<int> seta(a.begin(), a.end());
    for (int p: b) if (seta.count(p-1)) return true;
    return false;
}

std::string QueryEngine::make_snippet(const std::string& content, const std::vector<std::string>& terms, size_t maxlen) {
    std::string lc = Tokenizer::normalize(content);
    std::string needle;
    if (!terms.empty()) needle = terms[0];
    size_t pos = lc.find(needle);
    size_t start = (pos==std::string::npos? 0 : (pos>30? pos-30:0));
    if (content.size() <= maxlen) return content;
    return content.substr(start, maxlen) + (content.size()>start+maxlen? "..." : "");
}

std::vector<SearchResult> QueryEngine::search(const std::string& query, size_t k) {
    auto toks = split_query_terms(query);
    // Build list of term tokens (excluding OR and phrases handled later)
    std::vector<std::string> term_list;
    std::vector<std::pair<std::string,bool>> phrases; // phrase text, OR-joined?
    bool last_was_or = false;
    for (auto& t : toks) {
        if (t=="OR") { last_was_or=true; continue; }
        if (!t.empty() && t.front()=='"' && t.back()=='"' && t.size()>2) {
            phrases.push_back({t.substr(1,t.size()-2), last_was_or});
        } else {
            term_list.push_back(Tokenizer::normalize(t));
        }
        last_was_or=false;
    }
    // Score with BM25
    auto bm = ranker_.bm25(term_list);
    Ranker::normalize_inplace(bm);
    // Combine with PageRank
    std::unordered_map<int,double> fused = bm;
    for (auto& kv : pr_) fused[kv.first] += 0.15 * kv.second; // w_pr=0.15, BM25 already normalized
    // Phrase filtering: if phrases exist without OR, enforce them
    for (auto& ph : phrases) {
        std::vector<std::string> pterms;
        std::istringstream iss(ph.first);
        for (std::string s; iss>>s;) pterms.push_back(Tokenizer::normalize(s));
        if (pterms.size()<2) continue;
        // For each doc, verify consecutive positions chain
        std::vector<int> to_drop;
        for (auto& kv : fused) {
            int d = kv.first;
            bool ok = true;
            const TermInfo* prev = idx_.get(pterms[0]);
            if (!prev) { ok=false; }
            std::vector<int> prevPos;
            if (ok) {
                // find posting for d
                bool found=false;
                for (auto& p: prev->postings) if (p.doc_id==d) { prevPos=p.positions; found=true; break; }
                if (!found) ok=false;
            }
            for (size_t i=1; ok && i<pterms.size(); ++i) {
                const TermInfo* ti = idx_.get(pterms[i]);
                if (!ti) { ok=false; break; }
                std::vector<int> curPos; bool found=false;
                for (auto& p: ti->postings) if (p.doc_id==d) { curPos=p.positions; found=true; break; }
                if (!found || !phrase_match(prevPos, curPos)) { ok=false; break; }
                prevPos = curPos;
            }
            if (!ok && !ph.second) to_drop.push_back(d); // enforce unless OR-joined
            else if (ok && ph.second) fused[d] += 0.05; // slight bonus for OR phrase matches
        }
        for (int d : to_drop) fused.erase(d);
    }
    // Collect and sort
    std::vector<SearchResult> results;
    for (auto& kv : fused) {
        const auto& doc = store_.get(kv.first);
        results.push_back({kv.first, kv.second, doc.title.empty()? doc.filename : doc.title,
                           make_snippet(doc.content, term_list)});
    }
    std::sort(results.begin(), results.end(), [](const auto& a, const auto& b){
        if (a.score==b.score) return a.doc_id < b.doc_id;
        return a.score > b.score;
    });
    if (results.size()>k) results.resize(k);
    return results;
}
