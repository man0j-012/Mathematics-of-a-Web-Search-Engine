#pragma once
#include <string>
#include <vector>
#include <unordered_set>

struct Token {
    std::string term;
    int position; // token index within the document
};

class Tokenizer {
public:
    explicit Tokenizer(bool enable_stem=false);
    std::vector<Token> tokenize(const std::string& text) const;
    static std::string normalize(const std::string& s);
private:
    bool stem_;
    std::unordered_set<std::string> stop_;
    std::string stem(const std::string& s) const;
};
