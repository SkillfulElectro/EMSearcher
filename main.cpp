#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

#include "libs/porter/porter.h"
#include "libs/EMSearcher/EMSearcher.h"
#include "libs/util/util.h"

int main() {

  /// you can setup porter and any other search methods on top of this engine
  /// as an example i used ANSI Porter here
  std::cout << "insert your dir full path : C:/sample/path \n";
  std::string dir_path;
  std::cin >> dir_path;

  auto files = getFilesInDirectory(dir_path);
  auto contents = getFileContents(files);

  EMSearcher searcher;

  std::unordered_set<char> identifier = {' ', '-', ',', '<', ':' ,
                                         '>', '(', ')', '{', '}' , '\n' , '\t'};

  std::unordered_set<std::string> stop_words = {"of" , "the"};
  std::unordered_map<std::string, std::vector<std::string>> synonyms;

  searcher.configure(identifier, stop_words, synonyms);

  
  PorterStemmer stemmer;
  std::cout << "initializing the SyS \n";
  for (int i{0}; i < contents.size(); ++i) {
    auto tmp = stemmer.stem(contents[i], identifier);
    // std::cout << "Porterized : " << tmp << '\n';
    searcher.index_document(tmp, i);
  }


  std::string query;
  std::cout << "insert your query : ";
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

  std::getline(std::cin, query);

  // searcher.worder.print_word_counts();
  auto results = searcher.search(stemmer.stem(query, identifier));
  std::cout << "Number of results : " << results.size() << '\n';
  std::cout << "Searching ...\n";
  for (const auto &result : results) {
    std::cout << "file path : " << files[result.doc_id]
              << ", negative score : " << result.negative_score << std::endl;
  }

  return 0;
}
