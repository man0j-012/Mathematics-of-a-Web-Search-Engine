#include "Tokenizer.h"
#include <algorithm>
#include <cctype>
#include <locale>

static std::unordered_set<std::string> defaultStops() {
    return {"a","an","the","and","or","to","of","in","on","for","is","it","with","as","by","be"};
}

Tokenizer::Tokenizer(bool enable_stem): stem_(enable_stem), stop_(defaultStops()) {}

std::string Tokenizer::normalize(const std::string& s) {
    std::string out;
    out.reserve(s.size());
    for (unsigned char c : s) {
        if (std::isalnum(c)) out.push_back(std::tolower(c));
        else out.push_back(' ');
    }
    return out;
}

std::string Tokenizer::stem(const std::string& s) const {
    // ultra-light "stemmer": strip common suffixes; purely demonstrational
    std::string t = s;
    auto ends_with = [&](const std::string& suf){
        return t.size()>=suf.size() && t.compare(t.size()-suf.size(), suf.size(), suf)==0;
    };
    if (t.size()>4) {
        if (ends_with("ing")) t.resize(t.size()-3);
        else if (ends_with("ed")) t.resize(t.size()-2);
        else if (ends_with("es")) t.resize(t.size()-2);
        else if (ends_with("s") && t.size()>3) t.resize(t.size()-1);
    }
    return t;
}

std::vector<Token> Tokenizer::tokenize(const std::string& text) const {
    std::string norm = normalize(text);
    std::vector<Token> toks;
    std::string cur;
    int pos = 0;
    for (size_t i=0;i<=norm.size();++i){
        char ch = (i<norm.size()? norm[i] : ' ');
        if (std::isalnum(static_cast<unsigned char>(ch))) cur.push_back(ch);
        else {
            if (!cur.empty()) {
                std::string term = stem_ ? stem(cur) : cur;
                if (!stop_.count(term)) {
                    toks.push_back({term, pos});
                }
                cur.clear();
                ++pos;
            }
        }
    }
    return toks;
}
