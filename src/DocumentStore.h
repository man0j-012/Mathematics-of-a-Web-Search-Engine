#pragma once
#include <string>
#include <vector>
#include <unordered_map>

struct Document {
    int id;
    std::string filename;
    std::string title;
    std::string content;
    int length_tokens = 0;
};

class DocumentStore {
public:
    int add(const std::string& filename, const std::string& title, const std::string& content, int length_tokens);
    const Document& get(int id) const;
    int size() const { return (int)docs_.size(); }
    double avg_doc_len() const;
    const std::vector<Document>& all() const { return docs_; }
private:
    std::vector<Document> docs_;
};
