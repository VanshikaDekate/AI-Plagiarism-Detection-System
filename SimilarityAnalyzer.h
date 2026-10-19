#ifndef SIMILARITY_ANALYZER_H
#define SIMILARITY_ANALYZER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <cstddef>

using namespace std;

struct SimilarityResult
{
    string filename;

    double jaccard = 0.0;
    double cosine = 0.0;
    double ngram = 0.0;
    double finalScore = 0.0;

    vector<string> matchingPhrases;
};

class Document;

class SimilarityAnalyzer
{
private:

    // Quick Sort functions
    std::size_t partition(
        vector<SimilarityResult>& results,
        std::size_t low,
        std::size_t high
    );

    void quickSort(
        vector<SimilarityResult>& results,
        std::size_t low,
        std::size_t high
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