# Web Search Engine

A compact, self-contained C++17 search engine you can build and run locally. It demonstrates the mathematics behind search: tokenization, an inverted index, TF‑IDF/BM25 scoring, PageRank, and a simple rank fusion.

## Features
- **Tokenizer** with lowercasing, ASCII folding, stopword removal, and optional Porter stemming (very light).
- **Inverted Index** with positional postings and document frequencies.
- **Ranking** using BM25 (tunable) and **PageRank** over a tiny link graph parsed from files.
- **Query Engine** supporting:
  - Space-separated terms (AND semantics by default).
  - `OR` operator (upper-case) for disjunctions, e.g., `apple OR banana`.
  - Quotes `"..."` for simple phrase matching (requires consecutive positions).
- **REPL** to run many queries quickly.
- **Zero external deps** — just CMake and a C++17 compiler.

## Quick Start
```bash
mkdir build && cd build
cmake .. && cmake --build . -j
./tiny_search --data ../data --query "distributed systems"
# or start interactive shell
./tiny_search --data ../data
```

## Data Format
Put plain-text docs in a folder. The first line starting with `TITLE:` is the title (optional).
Link other docs using `LINK: filename.txt` lines (zero or more). Everything else is content.

Example:
```
TITLE: PageRank Basics
LINK: doc2.txt
LINK: doc3.txt
PageRank models the probability of a random surfer landing on a page...
```

## Math Summary

**Inverted Index.** For term *t* in document *d* with term frequency `tf(t,d)` and document frequency `df(t)`, we store positions to enable phrases.
IDF is computed as:  
\[
\mathrm{idf}(t) = \log \frac{N - df(t) + 0.5}{df(t) + 0.5 + 1e-9}
\]
with *N* = number of docs.

**BM25.** For query *q* and doc *d*:
\[
\mathrm{BM25}(q,d) = \sum_{t \in q} \mathrm{idf}(t) \cdot \frac{ tf(t,d) \cdot (k_1+1) }{ tf(t,d) + k_1 \cdot \left(1 - b + b \cdot \frac{|d|}{\mathrm{avgdl}} \right) }
\]
Defaults: \(k_1=1.2, b=0.75\).

**PageRank.** Power iteration on the link graph with damping \(\alpha=0.85\). Dangling mass redistributed uniformly.

**Fusion.** Final score = `w_bm25 * norm(BM25) + w_pr * norm(PageRank)` with defaults `w_bm25=0.85, w_pr=0.15`.

## License
MIT — do anything, attribution appreciated.
