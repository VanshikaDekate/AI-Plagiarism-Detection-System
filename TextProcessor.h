#ifndef TEXTPROCESSOR_H
#define TEXTPROCESSOR_H

#include <string>
#include <vector>

using namespace std;

/*
============================================================
CLASS: TextProcessor
============================================================

RESPONSIBILITY:
    Performs common text preprocessing.

ALGORITHMS:
    1. Text normalization
    2. Tokenization
    3. N-gram generation

DATA STRUCTURES:
    vector<string>
        Used to store ordered words and n-grams.

    string
        Used to store raw and cleaned text.

This corresponds to the preprocessing stage described
in the project document.
============================================================
*/

class TextProcessor
{
public:

    // -------------------------------------------------------
    // ALGORITHM: Text Normalization
    //
    // Converts text to lowercase and normalizes punctuation
    // and whitespace.
    //
    // Data structure:
    //     string
    //
    // Time Complexity:
    //     O(N)
    //     N = number of characters in the input text
    // -------------------------------------------------------
    string cleanText(const string& text);


    // -------------------------------------------------------
    // ALGORITHM: Tokenization
    //
    // Splits cleaned text into individual words.
    //
    // Data structure:
    //     vector<string>
    //
    // vector preserves the original ordering of words.
    //
    // Time Complexity:
    //     O(N)
    // -------------------------------------------------------
    vector<string> tokenize(const string& text);


    // -------------------------------------------------------
    // ALGORITHM: N-Gram Generation
    //
    // Creates sequences of N consecutive words.
    //
    // Example:
    //
    // Words:
    // artificial intelligence is transforming
    //
    // Trigrams:
    // artificial intelligence is
    // intelligence is transforming
    //
    // Data structure:
    //     vector<string>
    //
    // Time Complexity:
    //     O(W)
    //     W = number of words
    // -------------------------------------------------------
    vector<string> generateNGrams(
        const vector<string>& words,
        int n
    );
};

#endif