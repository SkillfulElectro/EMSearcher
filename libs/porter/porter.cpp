#include "porter.h"

std::string PorterStemmer::stem(const std::string& word, const std::unordered_set<char>& identifier) {
    auto words = split_word_index(word, identifier);
    std::string full_text = "";

    for (int i = 0; i < words.size(); ++i) {
        this->b = words[i]; // Copy input word
        k0 = 0;
        k = b.length() - 1;
        j = k;

        if (k <= k0 + 1) {
            full_text += words[i];
            full_text += ' ';
            continue;
        }

        step1ab();
        step1c();
        step2();
        step3();
        step4();
        step5();

        full_text += b.substr(0, k + 1); // Return stemmed string
        full_text += ' ';
    }
    return full_text;
}

bool PorterStemmer::cons(int i) {
    switch (b[i]) {
        case 'a': case 'e': case 'i': case 'o': case 'u': return false;
        case 'y': return (i == k0) ? true : !cons(i - 1);
        default: return true;
    }
}

int PorterStemmer::m() {
    int n = 0, i = k0;
    while (true) {
        if (i > j) return n;
        if (!cons(i)) break; 
        i++;
    }
    i++;
    while (true) {
        while (true) {
            if (i > j) return n;
            if (cons(i)) break; 
            i++;
        }
        i++; n++;
        while (true) {
            if (i > j) return n;
            if (!cons(i)) break; 
            i++;
        }
        i++;
    }
}

bool PorterStemmer::vowelinstem() {
    for (int i = k0; i <= j; i++) {
        if (!cons(i)) return true;
    }
    return false;
}

bool PorterStemmer::doublec(int j) {
    if (j < k0 + 1) return false;
    if (b[j] != b[j - 1]) return false;
    return cons(j);
}

bool PorterStemmer::cvc(int i) {
    if (i < k0 + 2 || !cons(i) || cons(i - 1) || !cons(i - 2)) return false;
    char ch = b[i];
    return ch != 'w' && ch != 'x' && ch != 'y';
}

bool PorterStemmer::ends(const char* s) {
    int length = strlen(s);
    if (s[length - 1] != b[k]) return false;
    if (length > k - k0 + 1) return false;
    if (b.compare(k - length + 1, length, s) != 0) return false;
    j = k - length;
    return true;
}

void PorterStemmer::setto(const char* s) {
    int length = strlen(s);
    b.replace(j + 1, k - j, s);
    k = j + length;
}

void PorterStemmer::r(const char* s) {
    if (m() > 0) setto(s);
}

void PorterStemmer::step1ab() {
    if (b[k] == 's') {
        if (ends("sses")) k -= 2;
        else if (ends("ies")) setto("i");
        else if (b[k - 1] != 's') k--;
    }
    if (ends("eed")) {
        if (m() > 0) k--;
    } else if ((ends("ed") || ends("ing")) && vowelinstem()) {
        k = j;
        if (ends("at")) setto("ate");
        else if (ends("bl")) setto("ble");
        else if (ends("iz")) setto("ize");
        else if (doublec(k)) {
            k--;
            char ch = b[k];
            if (ch == 'l' || ch == 's' || ch == 'z') k++;
        } else if (m() == 1 && cvc(k)) setto("e");
    }
}

void PorterStemmer::step1c() {
    if (ends("y") && vowelinstem()) b[k] = 'i';
}

void PorterStemmer::step2() {
    switch (b[k - 1]) {
        case 'a': if (ends("ational")) { r("ate"); break; }
                  if (ends("tional")) { r("tion"); break; } break;
        case 'c': if (ends("enci")) { r("ence"); break; }
                  if (ends("anci")) { r("ance"); break; } break;
        case 'e': if (ends("izer")) { r("ize"); break; } break;
        case 'l': if (ends("bli")) { r("ble"); break; }
                  if (ends("alli")) { r("al"); break; } break;
        case 'o': if (ends("ization")) { r("ize"); break; }
                  if (ends("ation")) { r("ate"); break; } break;
        case 's': if (ends("iveness")) { r("ive"); break; } break;
        case 't': if (ends("biliti")) { r("ble"); break; } break;
    }
}

void PorterStemmer::step3() {
    if (b[k] == 'e' && ends("ative")) setto("");
}

void PorterStemmer::step4() {
    if (ends("ement")) setto("");
    else if (ends("ment")) setto("");
    else if (ends("ive")) setto("");
}

void PorterStemmer::step5() {
    if (b[k] == 'e') {
        int a = m();
        if (a > 1 || (a == 1 && !cvc(k - 1))) k--;
    }
    if (doublec(k) && b[k] == 'l' && m() > 1) k--;
}
