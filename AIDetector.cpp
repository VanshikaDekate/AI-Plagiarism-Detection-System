#include "AIDetector.h"
#include "TextProcessor.h"

#include <algorithm>
#include <cmath>
#include <cctype>
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

    unordered_map<string, size_t> frequency;

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

    double totalCharacters = 0.0;

    for (const string& word : words)
    {
        totalCharacters += static_cast<double>(word.length());
    }

    features.avgWordLength =
        totalCharacters / static_cast<double>(words.size());


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
        / static_cast<double>(words.size());


    // =======================================================
    // FEATURE 3:
    // Repetition Ratio
    //
    // Counts words that appear more than once.
    // =======================================================

    size_t repeatedWords = 0;

    for (const auto& pair : frequency)
    {
        if (pair.second > 1)
        {
            repeatedWords += pair.second - 1;
        }
    }

    features.repetitionRatio =
        static_cast<double>(repeatedWords)
        / static_cast<double>(words.size());


    // =======================================================
    // FEATURE 4:
    // Function Word Rate
    // =======================================================

    size_t functionWordCount = 0;

    for (const string& word : words)
    {
        if (isFunctionWord(word))
        {
            functionWordCount++;
        }
    }

    features.functionWordRate =
        static_cast<double>(functionWordCount)
        / static_cast<double>(words.size());


    // =======================================================
    // FEATURE 5:
    // Punctuation Rate
    //
    // Counts punctuation characters relative to text size.
    // =======================================================

    size_t punctuationCount = 0;

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
            / static_cast<double>(text.size());
    }


    // =======================================================
    // SENTENCE ANALYSIS
    //
    // DATA STRUCTURE:
    // vector<int>
    //
    // Stores number of words in each sentence.
    // =======================================================

    vector<size_t> sentenceLengths;

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
                sentenceLengths.push_back(sentenceWords.size());
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
            sentenceLengths.push_back(sentenceWords.size());
        }
    }


    if (!sentenceLengths.empty())
    {
        // ===================================================
        // FEATURE 6:
        // Average Sentence Length
        // ===================================================

        double sum = 0.0;
        for (size_t length : sentenceLengths)
        {
            sum += static_cast<double>(length);
        }

        double mean =
            sum / static_cast<double>(sentenceLengths.size());

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

        for (size_t length : sentenceLengths)
        {
            const double sentenceLength =
                static_cast<double>(length);
            variance +=
                (sentenceLength - mean) *
                (sentenceLength - mean);
        }

        variance /= static_cast<double>(sentenceLengths.size());

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
    if (!isfinite(f.avgSentenceLength) ||
        !isfinite(f.sentenceLengthVariance) ||
        !isfinite(f.avgWordLength) ||
        !isfinite(f.vocabularyRichness) ||
        !isfinite(f.repetitionRatio) ||
        !isfinite(f.punctuationRate) ||
        !isfinite(f.functionWordRate))
    {
        return 0.0;
    }

    // Do not score text with no measurable linguistic features.
    if (f.avgSentenceLength <= 0.0 &&
        f.sentenceLengthVariance <= 0.0 &&
        f.avgWordLength <= 0.0 &&
        f.vocabularyRichness <= 0.0 &&
        f.repetitionRatio <= 0.0 &&
        f.punctuationRate <= 0.0 &&
        f.functionWordRate <= 0.0)
    {
        return 0.0;
    }

    const auto normalize = [](double value, double scale)
    {
        return clamp(value / scale, 0.0, 1.0);
    };

    const double sentenceLengthScore =
        normalize(f.avgSentenceLength, 30.0);
    const double varianceScore =
        normalize(f.sentenceLengthVariance, 100.0);
    const double wordLengthScore =
        normalize(f.avgWordLength, 10.0);
    const double vocabularyScore =
        clamp(f.vocabularyRichness, 0.0, 1.0);
    const double repetitionScore =
        clamp(f.repetitionRatio, 0.0, 1.0);
    const double punctuationScore =
        normalize(f.punctuationRate, 0.1);
    const double functionWordScore =
        normalize(f.functionWordRate, 0.5);

    double score =
          0.20 * sentenceLengthScore
        + 0.10 * varianceScore
        + 0.10 * wordLengthScore
        + 0.20 * vocabularyScore
        + 0.15 * (1.0 - repetitionScore)
        + 0.10 * punctuationScore
        + 0.15 * functionWordScore;

    return clamp(score * 100.0, 0.0, 100.0);
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