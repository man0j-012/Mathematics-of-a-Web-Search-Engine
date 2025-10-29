#include "DocumentStore.h"
#include <stdexcept>

int DocumentStore::add(const std::string& filename, const std::string& title, const std::string& content, int length_tokens) {
    int id = (int)docs_.size();
    docs_.push_back(Document{id, filename, title, content, length_tokens});
    return id;
}

const Document& DocumentStore::get(int id) const {
    if (id<0 || id>=(int)docs_.size()) throw std::runtime_error("bad doc id");
    return docs_[id];
}

double DocumentStore::avg_doc_len() const {
    if (docs_.empty()) return 0.0;
    long long sum = 0;
    for (auto& d: docs_) sum += d.length_tokens;
    return (double)sum / (double)docs_.size();
}
