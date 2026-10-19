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
    // First Menu of Code
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


    string originalText; // Stores the entire user input, including line breaks.
    string line; // Temporary variable to read each line of input.


    while (getline(cin, line)) // Read input line by line until "END" is entered.
    {
        if (line == "END") // Check for termination condition.
            break; // Exit the loop if "END" is entered.


        if (!originalText.empty()) // Add a newline character before appending the next line, if originalText is not empty.
            originalText += "\n"; // Preserve line breaks in the original text.


        originalText += line; // Append the current line to the original text.
    }


    if (originalText.empty()) // Check if the user entered any text.
    {
        cout << "\nNo text entered.\n";
        return 0; // Exit the program if no text was entered.
    }
    {
        cout << "\nNo text entered.\n"; // Inform the user that no text was entered.
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

    TextProcessor processor; // Create an instance of the TextProcessor class to handle text preprocessing.


    string cleanedText =
        processor.cleanText(originalText); // Normalize the original text by converting it to lowercase and standardizing punctuation and whitespace.


    vector<string> words =
        processor.tokenize(cleanedText); // Tokenize the cleaned text into individual words, preserving their original order.


    vector<string> ngrams =
        processor.generateNGrams(words, 3); // Generate trigrams (3-word sequences) from the tokenized words for further analysis.


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

    unordered_map<string, int> frequency; // Create a hash table to store the frequency of each word in the tokenized text. The key is the word, and the value is its count.


    for (const string& word : words) // Iterate through each tokenized word to populate the frequency table.
    {
        frequency[word]++; // Increment the count for the current word in the frequency table. If the word does not exist, it is automatically added with a count of 1.
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

    AIDetector aiDetector;// Create an instance of the AIDetector class to analyze the text for AI-like patterns.


    FeatureVector features =
        aiDetector.extractFeatures(originalText); // Extract various linguistic features from the original text, such as average sentence length, vocabulary richness, and punctuation rate.


    double aiScore =
        aiDetector.calculateScore(features); // Calculate an AI-likeness score based on the extracted features, resulting in a numerical value between 0 and 100.


    string aiClassification =
        aiDetector.classify(aiScore); // Classify the AI-likeness score into a readable category (e.g., "Higher AI-like pattern") based on predefined thresholds.


    // =======================================================
    // STEP 5: LOAD REFERENCE DOCUMENTS
    //
    // DATA STRUCTURE:
    // vector<Document>
    //
    // ALGORITHM:
    // Directory traversal + file processing.
    // =======================================================

    DocumentManager documentManager; // Create an instance of the DocumentManager class to handle loading and managing reference documents for plagiarism analysis.


    documentManager.loadDocuments( // Load all text files from the specified directory into the DocumentManager for later analysis.
        "data/reference"
    );


    const vector<Document>& documents =
        documentManager.getDocuments(); // Retrieve the loaded reference documents from the DocumentManager for use in plagiarism analysis.


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

    SimilarityAnalyzer analyzer; // Create an instance of the SimilarityAnalyzer class to perform plagiarism analysis by comparing the submitted text against reference documents.


    vector<SimilarityResult> results = // Analyze the submitted text against all reference documents, calculating similarity scores using Jaccard, Cosine, and N-gram methods, and returning a vector of results.
    analyzer.analyzeAll( // Pass the necessary data structures to the analyzeAll function for plagiarism analysis.
        words, // Tokenized words from the submitted text.
        frequency, // Word frequency table from the submitted text.
        ngrams, // Generated trigrams from the submitted text.
        documents // Reference documents to compare against.
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

    ReportGenerator report; // Create an instance of the ReportGenerator class to display the results of AI detection and plagiarism analysis in a user-friendly format.


    report.showCombinedReport( // Display a combined report that includes AI detection results and plagiarism analysis results, providing a comprehensive overview of the submitted text's characteristics and similarities to reference documents.
        features, // Linguistic features extracted from the submitted text.
        aiScore, // Calculated AI-likeness score for the submitted text.
        aiClassification, // Classification of the AI-likeness score into a readable category.
        results // Plagiarism analysis results.
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