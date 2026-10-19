#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

using namespace std;


/*
============================================================
CLASS: Document
============================================================

REPRESENTS:
    One reference document used for plagiarism detection.

DATA STRUCTURES:

    string
        Stores filename and original content.

    vector<string>
        Stores words in their original order.

    unordered_map<string, int>
        Stores word frequencies.

        Example:
            "ai" -> 5
            "system" -> 3

        Average lookup:
            O(1)

    unordered_set<string>
        Stores unique n-grams.

        Average lookup:
            O(1)

============================================================
*/

class Document
{
public:

    string filename;
    string content;

    vector<string> words;

    // HASH TABLE
    // Key   = word
    // Value = frequency
    unordered_map<string, int> frequency;

    // HASH SET
    // Stores unique trigrams.
    unordered_set<string> ngrams;


    Document();


    Document(
        const string& filename,
        const string& content
    );


    /*
    Builds the internal data structures:

        words
        frequency
        ngrams

    after the document content has been loaded.
    */
    void buildIndex();
};

#endif