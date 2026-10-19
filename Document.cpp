#include "Document.h"
#include "TextProcessor.h"

using namespace std;


/*
============================================================
DEFAULT CONSTRUCTOR
============================================================
*/

Document::Document()
{
}


/*
============================================================
PARAMETERIZED CONSTRUCTOR
============================================================
*/

Document::Document(
    const string& filename,
    const string& content)
{
    this->filename = filename;
    this->content = content;

    buildIndex();
}


/*
============================================================
FUNCTION: buildIndex()
============================================================

PURPOSE:
    Convert a document into structures that allow fast
    similarity calculations.

DATA STRUCTURES:

    vector<string>
        Stores ordered words.

    unordered_map<string,int>
        Stores word frequencies.

    unordered_set<string>
        Stores unique n-grams.

ALGORITHMS:

    1. Text preprocessing
    2. Tokenization
    3. Frequency counting
    4. N-gram generation

TIME COMPLEXITY:
    Approximately O(N)
    for fixed-size trigrams.

============================================================
*/

void Document::buildIndex()
{
    TextProcessor processor;

    // -------------------------------------------------------
    // STEP 1: Clean the text
    // -------------------------------------------------------

    string cleaned = processor.cleanText(content);


    // -------------------------------------------------------
    // STEP 2: Tokenization
    //
    // DATA STRUCTURE:
    // vector<string>
    // -------------------------------------------------------

    words = processor.tokenize(cleaned);


    // -------------------------------------------------------
    // STEP 3: Frequency Counting
    //
    // DATA STRUCTURE:
    // unordered_map<string,int>
    //
    // ALGORITHM:
    // Hash-table based frequency counting.
    //
    // Average insertion/update:
    // O(1)
    // -------------------------------------------------------

    frequency.clear();

    for (const string& word : words)
    {
        frequency[word]++;
    }


    // -------------------------------------------------------
    // STEP 4: Generate trigrams
    //
    // ALGORITHM:
    // Sliding-window N-gram generation.
    // -------------------------------------------------------

    vector<string> generatedNGrams =
        processor.generateNGrams(words, 3);


    // -------------------------------------------------------
    // STEP 5: Store UNIQUE trigrams
    //
    // DATA STRUCTURE:
    // unordered_set<string>
    //
    // WHY?
    // We only need to know whether a trigram exists when
    // calculating n-gram overlap.
    // -------------------------------------------------------

    ngrams.clear();

    for (const string& gram : generatedNGrams)
    {
        ngrams.insert(gram);
    }
}