#ifndef SIMILARITY_ANALYZER_H
#define SIMILARITY_ANALYZER_H

#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

struct SimilarityResult
{
    string filename;

    double jaccard;
    double cosine;
    double ngram;
    double finalScore;

    vector<string> matchingPhrases;
};

class Document;

class SimilarityAnalyzer
{
private:

    // Quick Sort functions
    int partition(
        vector<SimilarityResult>& results,
        int low,
        int high
    );

    void quickSort(
        vector<SimilarityResult>& results,
        int low,
        int high
    );

public:

    double jaccard(
        const vector<string>& words1,
        const vector<string>& words2
    );

    double cosine(
        const vector<string>& words1,
        const vector<string>& words2
    );

    double ngramSimilarity(
        const vector<string>& ngrams1,
        const vector<string>& ngrams2
    );

    SimilarityResult analyze(
        const vector<string>& submittedWords,
        const vector<string>& submittedNgrams,
        const string& filename,
        const vector<string>& documentWords,
        const vector<string>& documentNgrams
    );

    vector<SimilarityResult> analyzeAll(
        const vector<string>& submittedWords,
        const unordered_map<string, int>& frequency,
        const vector<string>& submittedNgrams,
        const vector<Document>& documents
    );
};

#endif