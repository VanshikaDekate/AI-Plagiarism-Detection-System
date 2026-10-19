
#ifndef PARAPHRASER_H
#define PARAPHRASER_H

#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

class Paraphraser
{
private:
    unordered_map<string, string> phraseMap;
    unordered_map<string, vector<string>> synonyms;

    string replacePhrases(const string& text);
    string replaceWords(const string& text);

public:
    Paraphraser();

    string rephrase(const string& text);
};

#endif
