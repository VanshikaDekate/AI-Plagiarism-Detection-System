#include "TextProcessor.h"

#include <algorithm>
#include <cctype>
#include <sstream>

using namespace std;


/*
============================================================
FUNCTION: cleanText()
============================================================

ALGORITHM:
    Text normalization

STEPS:
    1. Convert uppercase letters to lowercase.
    2. Keep alphabetic characters.
    3. Preserve sentence punctuation.
    4. Convert unnecessary punctuation into spaces.
    5. Normalize multiple spaces.

DATA STRUCTURE:
    string

TIME COMPLEXITY:
    O(N)
============================================================
*/

string TextProcessor::cleanText(const string& text) // Clean and normalize the input text for further processing.
{
    string cleaned; // Create a new string to hold the cleaned text.

    // Reserve memory to reduce repeated allocations.
    cleaned.reserve(text.size()); // Reserve space in the cleaned string to match the size of the input text, optimizing memory usage.

    for (char ch : text) // Iterate through each character in the input text to process it for normalization.
    {
        // Convert uppercase alphabetic characters to lowercase.
        if (isalpha(static_cast<unsigned char>(ch))) // Check if the character is an alphabetic letter.
        {
            cleaned += static_cast<char>( 
                tolower(static_cast<unsigned char>(ch)) // Convert the character to lowercase and append it to the cleaned string.
            ); // Convert the character to lowercase and append it to the cleaned string.
        }

        // Keep spaces.
        else if (isspace(static_cast<unsigned char>(ch)))
        {
            cleaned += ' ';
        }

        // Keep sentence-ending punctuation.
        else if (ch == '.' || ch == '!' || ch == '?')
        {
            cleaned += ch;
        }

        // Other punctuation is replaced by a space.
        else
        {
            cleaned += ' ';
        }
    }

    // -------------------------------------------------------
    // ALGORITHM:
    // Remove repeated whitespace using stringstream.
    //
    // Data structure:
    //     stringstream
    // -------------------------------------------------------

    stringstream ss(cleaned);

    string word;
    string normalized;

    while (ss >> word)
    {
        if (!normalized.empty())
            normalized += " ";

        normalized += word;
    }

    return normalized;
}


/*
============================================================
FUNCTION: tokenize()
============================================================

ALGORITHM:
    Tokenization

PURPOSE:
    Convert text into individual words.

DATA STRUCTURE:
    vector<string>

WHY VECTOR?
    Words have an important order because:
        - sentence analysis needs ordering
        - n-grams need ordering
        - word positions may matter later

TIME COMPLEXITY:
    O(N)
============================================================
*/

vector<string> TextProcessor::tokenize(const string& text)
{
    vector<string> words;

    stringstream ss(text);
    string word;

    while (ss >> word)
    {
        // Remove sentence punctuation from token endings.
        while (!word.empty() &&
               (word.back() == '.' ||
                word.back() == '!' ||
                word.back() == '?'))
        {
            word.pop_back();
        }

        if (!word.empty())
        {
            words.push_back(word);
        }
    }

    return words;
}


/*
============================================================
FUNCTION: generateNGrams()
============================================================

ALGORITHM:
    Sliding-window N-gram generation.

Example for n = 3:

    "artificial intelligence is transforming"

Produces:

    artificial intelligence is
    intelligence is transforming

DATA STRUCTURE:
    vector<string>

TIME COMPLEXITY:
    O(W * N)

For a fixed n such as 3:
    O(W)

because n is constant.

============================================================
*/


vector<string> TextProcessor::generateNGrams(
    const vector<string>& words,
    int n)
{
    vector<string> ngrams;

    if (n <= 0)
        return ngrams;

    const size_t nSize = static_cast<size_t>(n);

    if (words.size() < nSize)
        return ngrams;

    // Sliding window.
    for (size_t i = 0; i <= words.size() - nSize; ++i)
    {
        string gram;

        for (size_t j = 0; j < nSize; ++j)
        {
            if (j > 0)
                gram += " ";

            gram += words[i + j];
        }

        ngrams.push_back(gram);
    }

    return ngrams;
}
