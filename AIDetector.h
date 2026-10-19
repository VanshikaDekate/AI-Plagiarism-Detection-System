#ifndef AIDETECTOR_H
#define AIDETECTOR_H

#include <string>
#include <vector>

using namespace std;


/*
============================================================
DATA STRUCTURE: FeatureVector
============================================================

PURPOSE:
    Stores numerical characteristics extracted from text.

This converts raw language into numerical information that
can be processed by a scoring algorithm.

FEATURES:

    avgSentenceLength
    sentenceLengthVariance
    avgWordLength
    vocabularyRichness
    repetitionRatio
    punctuationRate
    functionWordRate

============================================================
*/

struct FeatureVector
{
    double avgSentenceLength = 0.0;
    double sentenceLengthVariance = 0.0;

    double avgWordLength = 0.0;
    double vocabularyRichness = 0.0;

    double repetitionRatio = 0.0;
    double punctuationRate = 0.0;
    double functionWordRate = 0.0;
};


/*
============================================================
CLASS: AIDetector
============================================================

ALGORITHM:
    Feature extraction + weighted scoring.

This is the transparent first implementation described
in the project document.

IMPORTANT:
    The weights are project-development weights, NOT
    universal scientific constants.

============================================================
*/

class AIDetector
{
private:

    /*
    --------------------------------------------------------
    DATA STRUCTURE:
        vector<string>

    Stores common function words used by the feature
    extraction algorithm.
    --------------------------------------------------------
    */

    vector<string> functionWords;


    /*
    Check whether a word is a function word.
    */
    bool isFunctionWord(const string& word) const;


public:

    AIDetector();


    /*
    --------------------------------------------------------
    ALGORITHM:
        Feature Extraction

    Converts text into FeatureVector.
    --------------------------------------------------------
    */
    FeatureVector extractFeatures(
        const string& text
    ) const;


    /*
    --------------------------------------------------------
    ALGORITHM:
        Weighted scoring

    Returns an AI-likeness score between 0 and 100.
    --------------------------------------------------------
    */
    double calculateScore(
        const FeatureVector& features
    ) const;


    /*
    Converts numerical score into a readable classification.
    */
    string classify(double score) const;
};

#endif