#ifndef EM_PORTER_GUARD
#define EM_PORTER_GUARD

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <sstream>
#include <unordered_set>
#include "../util/util.h"

/// ANSI Porter stemmer
class PorterStemmer {
public:
    std::string stem(const std::string& word, const std::unordered_set<char>& identifier);

private:
    std::string b; // Working string
    int k, k0, j;

    bool cons(int i);
    int m();
    bool vowelinstem();
    bool doublec(int j);
    bool cvc(int i);
    bool ends(const char* s);
    void setto(const char* s);
    void r(const char* s);
    void step1ab();
    void step1c();
    void step2();
    void step3();
    void step4();
    void step5();
};

#endif // EM_PORTER_GUARD
