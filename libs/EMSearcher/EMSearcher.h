#ifndef EM_SEARCHER_GUARD
#define EM_SEARCHER_GUARD

#include <algorithm>
#include <cmath>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../util/util.h"

// Document Search State
struct DocSearchState {
    int doc_id;
    int negative_score = 0;
    int matched_word_count = 0;
    std::vector<int> ref_indices;

    void update_score(const std::vector<int> &new_indices);
};

// Core Engine of the app , used for indexing and searching
class EMSearcher {
private:
    std::unordered_map<std::string, std::vector<std::pair<int, std::vector<int>>>> word_index;
    std::unordered_map<std::string, std::vector<std::string>> synonyms;
    std::unordered_set<char> delimiters;
    std::unordered_set<std::string> skip_word_index;
    std::mutex index_mutex;

    void process_word(const std::string &word, int doc_id, int word_position);

public:

    void configure(const std::unordered_set<char> &delimiters_,
                   const std::unordered_set<std::string> &skip_word_index_,
                   const std::unordered_map<std::string, std::vector<std::string>> &synonyms_);


    void index_document(const std::string &text, int doc_id);


    std::vector<DocSearchState> search(const std::string &query);

    void print_index() const;
};

#endif // EM_SEARCHER_GUARD
