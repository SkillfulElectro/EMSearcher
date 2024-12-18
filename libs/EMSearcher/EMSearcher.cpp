#include "EMSearcher.h"

void DocSearchState::update_score(const std::vector<int> &new_indices) {
    if (ref_indices.empty()) {
        ref_indices = new_indices;
        return;
    }
    int min_distance = INT_MAX;
    for (int ref : ref_indices) {
        for (int idx : new_indices) {
            min_distance = std::min(min_distance, std::abs(ref - idx));
        }
    }
    negative_score += min_distance;
    ref_indices = {new_indices.front()};
}

void EMSearcher::configure(const std::unordered_set<char> &delimiters_,
                            const std::unordered_set<std::string> &skip_word_index_,
                            const std::unordered_map<std::string, std::vector<std::string>> &synonyms_) {
    delimiters = delimiters_;
    skip_word_index = skip_word_index_;
    synonyms = synonyms_;
}

void EMSearcher::index_document(const std::string &text, int doc_id) {
    std::string word_buffer;
    int word_position = 1;

    for (char ch : text) {
        if (delimiters.find(ch) == delimiters.end()) {
            word_buffer += ch;
        } else {
            if (!word_buffer.empty() && skip_word_index.find(word_buffer) == skip_word_index.end()) {
                process_word(word_buffer, doc_id, word_position);
                ++word_position;
            }
            word_buffer.clear();
        }
    }

    if (!word_buffer.empty() && skip_word_index.find(word_buffer) == skip_word_index.end()) {
        process_word(word_buffer, doc_id, word_position);
    }
}

void EMSearcher::process_word(const std::string &word, int doc_id, int word_position) {
    std::lock_guard<std::mutex> lock(index_mutex);

    auto &docs = word_index[word];
    if (!docs.empty() && docs.back().first == doc_id) {
        docs.back().second.push_back(word_position);
    } else {
        docs.push_back({doc_id, {word_position}});
    }
}

std::vector<DocSearchState> EMSearcher::search(const std::string &query) {
    auto query_word_index = split_word_index(query, delimiters);
    std::unordered_map<int, DocSearchState> results;

    std::mutex results_mutex;
    size_t thread_count = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;

    auto process_word_range = [&](size_t start, size_t end) {
        for (size_t i = start; i < end; ++i) {
            const auto &word = query_word_index[i];
            if (skip_word_index.count(word))
                continue;

            std::vector<std::pair<int, std::vector<int>>> docs;
            {
                std::lock_guard<std::mutex> lock(index_mutex);
                docs = word_index[word];

                if (synonyms.count(word)) {
                    for (const auto &synonym : synonyms[word]) {
                        auto &syn_docs = word_index[synonym];
                        docs.insert(docs.end(), syn_docs.begin(), syn_docs.end());
                    }
                }
            }

            std::lock_guard<std::mutex> lock(results_mutex);
            for (const auto &[doc_id, indices] : docs) {
                auto &state = results[doc_id];
                state.doc_id = doc_id;
                state.matched_word_count++;
                state.update_score(indices);
            }
        }
    };

    size_t chunk_size = query_word_index.size() / thread_count;
    for (size_t i = 0; i < thread_count; ++i) {
        size_t start = i * chunk_size;
        size_t end = (i == thread_count - 1) ? query_word_index.size() : start + chunk_size;
        threads.emplace_back(process_word_range, start, end);
    }

    for (auto &t : threads) {
        t.join();
    }

    std::vector<DocSearchState> sorted_results;
    for (auto &[_, state] : results) {
        state.negative_score += (query_word_index.size() - state.matched_word_count) * 4;
        sorted_results.push_back(state);
    }
    std::sort(sorted_results.begin(), sorted_results.end(),
              [](const auto &a, const auto &b) {
                  return a.negative_score < b.negative_score;
              });
    return sorted_results;
}

void EMSearcher::print_index() const {
    for (const auto &[word, docs] : word_index) {
        std::cout << word << ": ";
        for (const auto &[doc_id, indices] : docs) {
            std::cout << "[Doc " << doc_id << " Indices: ";
            for (int idx : indices)
                std::cout << idx << " ";
            std::cout << "] ";
        }
        std::cout << "\n";
    }
}
