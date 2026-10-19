#include "AIDetector.h"
#include "TextProcessor.h"

#include <algorithm>
#include <cmath>
#include <cctype>
#include <numeric>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

using namespace std;


/*
============================================================
CONSTRUCTOR
============================================================

DATA STRUCTURE:
    vector<string>

Stores common English function words.

These are used as one of the linguistic features.
============================================================
*/

AIDetector::AIDetector()
{
    functionWords =
    {
        "the", "a", "an",
        "is", "are", "was", "were",
        "of", "to", "in", "on",
        "for", "and", "or",
        "but", "with", "from",
        "by", "as", "at",
        "this", "that",
        "it", "be", "have",
        "has", "had"
    };
}


/*
============================================================
FUNCTION: isFunctionWord()
============================================================

DATA STRUCTURE:
    vector<string>

ALGORITHM:
    Linear search.

For this small fixed dictionary this is sufficient.

TIME COMPLEXITY:
    O(F)

F = number of function words.

============================================================
*/

bool AIDetector::isFunctionWord(
    const string& word) const
{
    return find(
        functionWords.begin(),
        functionWords.end(),
        word
    ) != functionWords.end();
}


/*
============================================================
FUNCTION: extractFeatures()
============================================================

ALGORITHM:
    Linguistic Feature Extraction

DATA STRUCTURES:

    vector<string>
        Stores words.

    unordered_map<string,int>
        Stores word frequencies.

    vector<int>
        Stores sentence lengths.

    unordered_set<string>
        Used for unique vocabulary.

============================================================
*/

FeatureVector AIDetector::extractFeatures(
    const string& text) const
{
    FeatureVector features;

    TextProcessor processor;


    // -------------------------------------------------------
    // Clean and tokenize the text.
    // -------------------------------------------------------

    string cleaned = processor.cleanText(text);

    vector<string> words =
        processor.tokenize(cleaned);


    if (words.empty())
        return features;


    // =======================================================
    // DATA STRUCTURE:
    // unordered_map<string,int>
    //
    // ALGORITHM:
    // Frequency counting using hashing.
    //
    // Average complexity:
    // O(N)
    // =======================================================

    unordered_map<string, int> frequency;

    for (const string& word : words)
    {
        frequency[word]++;
    }


    // =======================================================
    // DATA STRUCTURE:
    // unordered_set<string>
    //
    // Used to calculate vocabulary richness.
    // =======================================================

    unordered_set<string> uniqueWords;

    for (const string& word : words)
    {
        uniqueWords.insert(word);
    }


    // =======================================================
    // FEATURE 1:
    // Average Word Length
    // =======================================================

    double totalCharacters = 0;

    for (const string& word : words)
    {
        totalCharacters += word.length();
    }

    features.avgWordLength =
        totalCharacters / words.size();


    // =======================================================
    // FEATURE 2:
    // Vocabulary Richness
    //
    // Formula:
    //
    // unique words
    // -------------
    // total words
    // =======================================================

    features.vocabularyRichness =
        static_cast<double>(uniqueWords.size())
        / words.size();


    // =======================================================
    // FEATURE 3:
    // Repetition Ratio
    //
    // Counts words that appear more than once.
    // =======================================================

    int repeatedWords = 0;

    for (const auto& pair : frequency)
    {
        if (pair.second > 1)
        {
            repeatedWords += pair.second - 1;
        }
    }

    features.repetitionRatio =
        static_cast<double>(repeatedWords)
        / words.size();


    // =======================================================
    // FEATURE 4:
    // Function Word Rate
    // =======================================================

    int functionWordCount = 0;

    for (const string& word : words)
    {
        if (isFunctionWord(word))
        {
            functionWordCount++;
        }
    }

    features.functionWordRate =
        static_cast<double>(functionWordCount)
        / words.size();


    // =======================================================
    // FEATURE 5:
    // Punctuation Rate
    //
    // Counts punctuation characters relative to text size.
    // =======================================================

    int punctuationCount = 0;

    for (char ch : text)
    {
        if (ispunct(
                static_cast<unsigned char>(ch)))
        {
            punctuationCount++;
        }
    }

    if (!text.empty())
    {
        features.punctuationRate =
            static_cast<double>(punctuationCount)
            / text.size();
    }


    // =======================================================
    // SENTENCE ANALYSIS
    //
    // DATA STRUCTURE:
    // vector<int>
    //
    // Stores number of words in each sentence.
    // =======================================================

    vector<int> sentenceLengths;

    string currentSentence;

    stringstream ss(text);

    char ch;

    while (ss.get(ch))
    {
        currentSentence += ch;


        // Sentence boundary.
        if (ch == '.' ||
            ch == '!' ||
            ch == '?')
        {
            vector<string> sentenceWords =
                processor.tokenize(currentSentence);

            if (!sentenceWords.empty())
            {
                sentenceLengths.push_back(
                    sentenceWords.size()
                );
            }

            currentSentence.clear();
        }
    }


    // Handle text without ending punctuation.
    if (!currentSentence.empty())
    {
        vector<string> sentenceWords =
            processor.tokenize(currentSentence);

        if (!sentenceWords.empty())
        {
            sentenceLengths.push_back(
                sentenceWords.size()
            );
        }
    }


    if (!sentenceLengths.empty())
    {
        // ===================================================
        // FEATURE 6:
        // Average Sentence Length
        // ===================================================

        double sum = accumulate(
            sentenceLengths.begin(),
            sentenceLengths.end(),
            0.0
        );

        double mean =
            sum / sentenceLengths.size();

        features.avgSentenceLength = mean;


        // ===================================================
        // FEATURE 7:
        // Sentence Length Variance
        //
        // Formula:
        //
        // Σ(x - mean)²
        // -------------
        //      N
        // ===================================================

        double variance = 0.0;

        for (int length : sentenceLengths)
        {
            variance +=
                (length - mean) *
                (length - mean);
        }

        variance /= sentenceLengths.size();

        features.sentenceLengthVariance =
            variance;
    }


    return features;
}


/*
============================================================
FUNCTION: calculateScore()
============================================================

ALGORITHM:
    Weighted Feature Scoring

The project document proposes:

    AI Score =
        w1*f1 + w2*f2 + ... + wn*fn

Here we normalize selected features to approximate
0-1 ranges and combine them.

IMPORTANT:
    These weights are engineering choices for the course
    project. They are NOT universal scientific values.

============================================================
*/

double AIDetector::calculateScore(
    const FeatureVector& f) const
{
    /*
    --------------------------------------------------------
    Feature normalization
    --------------------------------------------------------

    These ranges are intentionally simple for the first
    implementation and can later be calibrated using a
    labeled dataset.
    --------------------------------------------------------
    */

    double sentenceLengthScore =
        min(f.avgSentenceLength / 30.0, 1.0);


    double varianceScore =
        min(f.sentenceLengthVariance / 100.0, 1.0);


    double wordLengthScore =
        min(f.avgWordLength / 10.0, 1.0);


    /*
    High vocabulary richness generally represents more
    diverse vocabulary, so we use it directly.
    */

    double vocabularyScore =
        min(f.vocabularyRichness, 1.0);


    double repetitionScore =
        min(f.repetitionRatio, 1.0);


    double punctuationScore =
        min(f.punctuationRate * 10.0, 1.0);


    double functionWordScore =
        min(f.functionWordRate * 2.0, 1.0);


    // -------------------------------------------------------
    // WEIGHTED SCORE
    // -------------------------------------------------------

    double score =
          0.20 * sentenceLengthScore
        + 0.10 * varianceScore
        + 0.10 * wordLengthScore
        + 0.20 * vocabularyScore
        + 0.15 * (1.0 - repetitionScore)
        + 0.10 * punctuationScore
        + 0.15 * functionWordScore;


    return score * 100.0;
}


/*
============================================================
FUNCTION: classify()
============================================================

ALGORITHM:
    Threshold-based classification.

This is NOT a machine-learning classifier.

It simply converts the numerical estimate into readable
categories.

============================================================
*/

string AIDetector::classify(double score) const
{
    if (score >= 70)
        return "Higher AI-like pattern";

    if (score >= 40)
        return "Moderate AI-like pattern";

    return "Lower AI-like pattern";
}