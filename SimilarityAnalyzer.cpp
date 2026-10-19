#include "SimilarityAnalyzer.h"
#include "Document.h"

#include <unordered_set>
#include <unordered_map>
#include <cmath>
#include <utility>

using namespace std;


// ============================================================
// JACCARD SIMILARITY
// ============================================================
//
// Jaccard measures the overlap between the unique words
// present in two documents.
//
// Formula:
//
//          |A ∩ B|
// J = -----------------
//          |A ∪ B|
//
// Data Structure:
//     unordered_set
//
// Hashing is used for fast membership checking.
//
// ============================================================

double SimilarityAnalyzer::jaccard(
    const vector<string>& words1,
    const vector<string>& words2
)
{
    unordered_set<string> set1(
        words1.begin(),
        words1.end()
    );

    unordered_set<string> set2(
        words2.begin(),
        words2.end()
    );

    int intersection = 0;

    // Linear traversal of set1.
    // Hash lookup is used to check whether each word
    // is also present in set2.
    for (const string& word : set1)
    {
        if (set2.find(word) != set2.end())
        {
            intersection++;
        }
    }

    int unionSize =
        static_cast<int>(set1.size()) +
        static_cast<int>(set2.size()) -
        intersection;

    if (unionSize == 0)
    {
        return 0.0;
    }

    return static_cast<double>(intersection)
           / unionSize;
}


// ============================================================
// COSINE SIMILARITY
// ============================================================
//
// Cosine similarity compares the frequency patterns of
// two documents.
//
// Data Structure:
//     unordered_map<string, int>
//
// The unordered_map stores:
//
//     word -> frequency
//
// Hashing allows average O(1) lookup.
//
// ============================================================

double SimilarityAnalyzer::cosine(
    const vector<string>& words1,
    const vector<string>& words2
)
{
    unordered_map<string, int> freq1;
    unordered_map<string, int> freq2;

    // Build frequency table for first text.
    for (const string& word : words1)
    {
        freq1[word]++;
    }

    // Build frequency table for second text.
    for (const string& word : words2)
    {
        freq2[word]++;
    }

    double dotProduct = 0.0;
    double magnitude1 = 0.0;
    double magnitude2 = 0.0;


    // Calculate dot product.
    for (const auto& item : freq1)
    {
        const string& word = item.first;
        int count1 = item.second;

        if (freq2.find(word) != freq2.end())
        {
            dotProduct +=
                count1 * freq2[word];
        }
    }


    // Calculate magnitude of first vector.
    for (const auto& item : freq1)
    {
        magnitude1 +=
            item.second * item.second;
    }


    // Calculate magnitude of second vector.
    for (const auto& item : freq2)
    {
        magnitude2 +=
            item.second * item.second;
    }


    if (magnitude1 == 0.0 ||
        magnitude2 == 0.0)
    {
        return 0.0;
    }


    return dotProduct /
           (sqrt(magnitude1) *
            sqrt(magnitude2));
}


// ============================================================
// N-GRAM SIMILARITY
// ============================================================
//
// Compares the 3-word sequences of the submitted text
// against the reference document.
//
// Data Structure:
//     unordered_set
//
// Hashing allows fast membership checking.
//
// ============================================================

double SimilarityAnalyzer::ngramSimilarity(
    const vector<string>& ngrams1,
    const vector<string>& ngrams2
)
{
    if (ngrams1.empty())
    {
        return 0.0;
    }

    unordered_set<string> referenceNgrams(
        ngrams2.begin(),
        ngrams2.end()
    );

    int matches = 0;

    for (const string& ngram : ngrams1)
    {
        if (referenceNgrams.find(ngram)
            != referenceNgrams.end())
        {
            matches++;
        }
    }

    return static_cast<double>(matches)
           / ngrams1.size();
}


// ============================================================
// ANALYZE ONE DOCUMENT
// ============================================================
//
// Calculates:
//
//     1. Jaccard similarity
//     2. Cosine similarity
//     3. N-gram similarity
//     4. Combined similarity score
//
// Combined score:
//
//     30% Jaccard
//     50% Cosine
//     20% N-gram
//
// ============================================================

SimilarityResult SimilarityAnalyzer::analyze(
    const vector<string>& submittedWords,
    const vector<string>& submittedNgrams,
    const string& filename,
    const vector<string>& documentWords,
    const vector<string>& documentNgrams
)
{
    SimilarityResult result;

    result.filename = filename;


    // --------------------------------------------------------
    // Jaccard similarity
    // --------------------------------------------------------

    result.jaccard =
        jaccard(
            submittedWords,
            documentWords
        );


    // --------------------------------------------------------
    // Cosine similarity
    // --------------------------------------------------------

    result.cosine =
        cosine(
            submittedWords,
            documentWords
        );


    // --------------------------------------------------------
    // N-gram similarity
    // --------------------------------------------------------

    result.ngram =
        ngramSimilarity(
            submittedNgrams,
            documentNgrams
        );


    // --------------------------------------------------------
    // Combined similarity score
    // --------------------------------------------------------

    result.finalScore =
        0.30 * result.jaccard +
        0.50 * result.cosine +
        0.20 * result.ngram;


    // --------------------------------------------------------
    // Find matching phrases
    // --------------------------------------------------------

    unordered_set<string> referenceNgrams(
        documentNgrams.begin(),
        documentNgrams.end()
    );

    for (const string& ngram : submittedNgrams)
    {
        if (referenceNgrams.find(ngram)
            != referenceNgrams.end())
        {
            result.matchingPhrases.push_back(
                ngram
            );
        }
    }

    return result;
}


// ============================================================
// QUICK SORT - PARTITION
// ============================================================
//
// We sort documents according to their final similarity
// score.
//
// Higher similarity = higher priority.
//
// Therefore:
//
//     90%
//     75%
//     62%
//     31%
//
// The algorithm sorts in DESCENDING order.
//
// Pivot:
//     Last element of the current partition.
//
// ============================================================

int SimilarityAnalyzer::partition(
    vector<SimilarityResult>& results,
    int low,
    int high
)
{
    // Choose the last element as pivot.
    double pivot =
        results[high].finalScore;

    int i = low - 1;


    // Traverse the current partition.
    for (int j = low; j < high; j++)
    {
        // Larger scores are placed before the pivot.
        if (results[j].finalScore > pivot)
        {
            i++;

            swap(
                results[i],
                results[j]
            );
        }
    }


    // Place pivot at its correct position.
    swap(
        results[i + 1],
        results[high]
    );

    return i + 1;
}


// ============================================================
// QUICK SORT
// ============================================================
//
// Algorithm:
//     1. Select a pivot.
//     2. Partition the data.
//     3. Recursively sort the left partition.
//     4. Recursively sort the right partition.
//
// Average Complexity:
//     O(n log n)
//
// Worst Case:
//     O(n²)
//
// ============================================================

void SimilarityAnalyzer::quickSort(
    vector<SimilarityResult>& results,
    int low,
    int high
)
{
    if (low < high)
    {
        // Partition the vector and obtain
        // the final position of the pivot.
        int pivotIndex =
            partition(
                results,
                low,
                high
            );


        // Sort left partition.
        quickSort(
            results,
            low,
            pivotIndex - 1
        );


        // Sort right partition.
        quickSort(
            results,
            pivotIndex + 1,
            high
        );
    }
}


// ============================================================
// ANALYZE ALL DOCUMENTS
// ============================================================
//
// Parameters:
//
//     submittedWords
//         Words from user input.
//
//     frequency
//         Word-frequency table created in main.cpp.
//
//     submittedNgrams
//         N-grams from user input.
//
//     documents
//         Reference document collection.
//
// The frequency parameter is retained so that this function
// remains compatible with the existing main.cpp interface.
//
// The cosine function currently builds its own frequency
// tables, so frequency is not directly required here.
//
// ============================================================

vector<SimilarityResult>
SimilarityAnalyzer::analyzeAll(
    const vector<string>& submittedWords,
    const unordered_map<string, int>& frequency,
    const vector<string>& submittedNgrams,
    const vector<Document>& documents
)
{
    // The frequency table is already constructed in main.cpp.
    // The current cosine implementation creates its own
    // frequency tables, so this parameter is intentionally
    // unused in this function.
    (void)frequency;


    vector<SimilarityResult> results;


    // --------------------------------------------------------
    // Analyze every reference document.
    // --------------------------------------------------------

    for (const Document& document : documents)
    {
        // Convert the document's n-gram set into a vector
        // so that it can be passed to analyze().
        vector<string> documentNgrams(
            document.ngrams.begin(),
            document.ngrams.end()
        );


        SimilarityResult result =
            analyze(
                submittedWords,
                submittedNgrams,
                document.filename,
                document.words,
                documentNgrams
            );


        results.push_back(result);
    }


    // --------------------------------------------------------
    // QUICK SORT
    //
    // Rank documents from highest similarity to lowest.
    // --------------------------------------------------------

    if (!results.empty())
    {
        quickSort(
            results,
            0,
            static_cast<int>(results.size()) - 1
        );
    }


    return results;
}