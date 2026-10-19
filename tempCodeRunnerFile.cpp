#include "TextProcessor.h"
#include "DocumentManager.h"
#include "AIDetector.h"
#include "SimilarityAnalyzer.h"
#include "ReportGenerator.h"

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;


/*
============================================================
MAIN PROGRAM
============================================================

PART 1 PIPELINE:

    USER INPUT
        ↓
    TEXT PREPROCESSING
        ↓
    AI FEATURE EXTRACTION
        ↓
    AI-LIKENESS SCORE
        ↓
    PLAGIARISM ANALYSIS
        ↓
    JACCARD + COSINE + N-GRAM
        ↓
    RANK DOCUMENTS
        ↓
    FINAL REPORT


DATA STRUCTURES USED:

    string
        Original user text.

    vector<string>
        Tokenized words and n-grams.

    unordered_map<string,int>
        Word-frequency table.

    vector<Document>
        Reference document collection.

    vector<SimilarityResult>
        Similarity results.

============================================================
*/

int main()
{
    cout << "========================================\n";
    cout << " AI & PLAGIARISM DETECTION SYSTEM\n";
    cout << "             PART 1\n";
    cout << "========================================\n";


    // =======================================================
    // STEP 1: GET USER INPUT
    //
    // DATA STRUCTURE:
    // string
    //
    // The original text is preserved.
    // =======================================================

    cout << "\nEnter your paragraph.\n";
    cout << "Type END on a new line when finished:\n\n";


    string originalText;
    string line;


    while (getline(cin, line))
    {
        if (line == "END")
            break;


        if (!originalText.empty())
            originalText += "\n";


        originalText += line;
    }


    if (originalText.empty())
    {
        cout << "\nNo text entered.\n";
        return 0;
    }


    // =======================================================
    // STEP 2: TEXT PREPROCESSING
    //
    // OBJECT:
    // TextProcessor
    //
    // ALGORITHMS:
    //     Normalization
    //     Tokenization
    //     N-gram generation
    // =======================================================

    TextProcessor processor;


    string cleanedText =
        processor.cleanText(originalText);


    vector<string> words =
        processor.tokenize(cleanedText);


    vector<string> ngrams =
        processor.generateNGrams(words, 3);


    // =======================================================
    // STEP 3: WORD FREQUENCY
    //
    // DATA STRUCTURE:
    // unordered_map<string,int>
    //
    // ALGORITHM:
    // Hash-based frequency counting.
    //
    // Average insertion/update:
    // O(1)
    // =======================================================

    unordered_map<string, int> frequency;


    for (const string& word : words)
    {
        frequency[word]++;
    }


    // =======================================================
    // DISPLAY BASIC INFORMATION
    // =======================================================

    cout << "\n----------------------------------------\n";
    cout << "TEXT PROCESSING RESULTS\n";
    cout << "----------------------------------------\n";

    cout << "Total words : "
         << words.size()
         << "\n";

    cout << "Total trigrams : "
         << ngrams.size()
         << "\n";


    // =======================================================
    // STEP 4: AI DETECTION
    //
    // ALGORITHM:
    // Feature extraction + weighted scoring.
    // =======================================================

    AIDetector aiDetector;


    FeatureVector features =
        aiDetector.extractFeatures(originalText);


    double aiScore =
        aiDetector.calculateScore(features);


    string aiClassification =
        aiDetector.classify(aiScore);


    // =======================================================
    // STEP 5: LOAD REFERENCE DOCUMENTS
    //
    // DATA STRUCTURE:
    // vector<Document>
    //
    // ALGORITHM:
    // Directory traversal + file processing.
    // =======================================================

    DocumentManager documentManager;


    documentManager.loadDocuments(
        "data/reference"
    );


    const vector<Document>& documents =
        documentManager.getDocuments();


    // =======================================================
    // STEP 6: PLAGIARISM ANALYSIS
    //
    // ALGORITHMS:
    //
    //     Jaccard similarity
    //     Cosine similarity
    //     N-gram matching
    //     Weighted combination
    //     Sorting
    // =======================================================

    SimilarityAnalyzer analyzer;


    vector<SimilarityResult> results =
    analyzer.analyzeAll(
        words,
        frequency,
        ngrams,
        documents
    );


    // =======================================================
    // STEP 7: GENERATE REPORT
    //
    // OBJECT:
    // ReportGenerator
    //
    // No analysis is performed here.
    // It simply presents the results.
    // =======================================================

    ReportGenerator report;


    report.showCombinedReport(
        features,
        aiScore,
        aiClassification,
        results
    );


    // =======================================================
    // PART 1 COMPLETE
    //
    // IMPORTANT:
    // We do NOT call a rephraser here.
    //
    // Part 2 will later add:
    //
    //     original text
    //          ↓
    //       rephrase
    //          ↓
    //     AI detection
    //          +
    //     plagiarism detection
    //          ↓
    //      before/after
    //
    // =======================================================

    return 0;
}