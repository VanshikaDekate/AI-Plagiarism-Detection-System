#include "ReportGenerator.h"

#include <iomanip>
#include <iostream>

using namespace std;


/*
============================================================
FUNCTION: showAIReport()
============================================================

Displays the numerical FeatureVector and AI score.

DATA STRUCTURE:
    FeatureVector

No new algorithm is performed here.
============================================================
*/

void ReportGenerator::showAIReport(
    const FeatureVector& features,
    double score,
    const string& classification) const
{
    cout << "\n";
    cout << "========================================\n";
    cout << "           AI DETECTION REPORT\n";
    cout << "========================================\n";


    cout << fixed << setprecision(2);


    cout << "\nLinguistic Features\n";
    cout << "----------------------------------------\n";

    cout << "Average Sentence Length : "
         << features.avgSentenceLength
         << "\n";

    cout << "Sentence Length Variance: "
         << features.sentenceLengthVariance
         << "\n";

    cout << "Average Word Length     : "
         << features.avgWordLength
         << "\n";

    cout << "Vocabulary Richness     : "
         << features.vocabularyRichness
         << "\n";

    cout << "Repetition Ratio        : "
         << features.repetitionRatio
         << "\n";

    cout << "Punctuation Rate        : "
         << features.punctuationRate
         << "\n";

    cout << "Function Word Rate      : "
         << features.functionWordRate
         << "\n";


    cout << "\nAI-Likeness Score       : "
         << score
         << "%\n";

    cout << "Classification          : "
         << classification
         << "\n";
}


/*
============================================================
FUNCTION: showPlagiarismReport()
============================================================

Displays ranked similarity results.

DATA STRUCTURE:
    vector<SimilarityResult>

The vector is already sorted by SimilarityAnalyzer.
============================================================
*/

void ReportGenerator::showPlagiarismReport(
    const vector<SimilarityResult>& results) const
{
    cout << "\n";
    cout << "========================================\n";
    cout << "       PLAGIARISM DETECTION REPORT\n";
    cout << "========================================\n";


    if (results.empty())
    {
        cout << "\nNo reference documents found.\n";
        return;
    }


    cout << fixed << setprecision(2);


    cout << "\nReference Document Ranking\n";
    cout << "----------------------------------------\n";


    int rank = 1;


    for (const auto& result : results)
    {
        cout << rank
             << ". "
             << result.filename
             << " -> "
             << result.finalScore * 100
             << "%\n";

        rank++;
    }


    // -------------------------------------------------------
    // Highest similarity document.
    // -------------------------------------------------------

    const SimilarityResult& best =
        results.front();


    cout << "\nMost Similar Document : "
         << best.filename
         << "\n";


    cout << "\nSimilarity Components\n";
    cout << "----------------------------------------\n";

    cout << "Jaccard Similarity : "
         << best.jaccard * 100
         << "%\n";

    cout << "Cosine Similarity  : "
         << best.cosine * 100
         << "%\n";

    cout << "N-Gram Similarity  : "
         << best.ngram * 100
         << "%\n";

    cout << "Final Similarity   : "
         << best.finalScore * 100
         << "%\n";


    // -------------------------------------------------------
    // Classification
    // -------------------------------------------------------

    if (best.finalScore >= 0.70)
    {
        cout << "\nClassification: "
             << "HIGH TEXTUAL SIMILARITY\n";
    }
    else if (best.finalScore >= 0.40)
    {
        cout << "\nClassification: "
             << "MODERATE TEXTUAL SIMILARITY\n";
    }
    else
    {
        cout << "\nClassification: "
             << "LOW TEXTUAL SIMILARITY\n";
    }


    // -------------------------------------------------------
    // Matching phrases.
    //
    // DATA STRUCTURE:
    // vector<string>
    // -------------------------------------------------------

    if (!best.matchingPhrases.empty())
    {
        cout << "\nMatching Phrases:\n";

        int count = 1;

        for (const string& phrase :
             best.matchingPhrases)
        {
            cout << count
                 << ". "
                 << phrase
                 << "\n";

            count++;


            // Prevent an absurdly huge report.
            if (count > 10)
                break;
        }
    }
    else
    {
        cout << "\nNo matching phrases found.\n";
    }
}


/*
============================================================
FUNCTION: showCombinedReport()
============================================================

Produces the complete Part 1 report.

============================================================
*/

void ReportGenerator::showCombinedReport(
    const FeatureVector& features,
    double aiScore,
    const string& classification,
    const vector<SimilarityResult>& results) const
{
    showAIReport(
        features,
        aiScore,
        classification
    );


    showPlagiarismReport(results);


    cout << "\n";
    cout << "========================================\n";
    cout << "             ANALYSIS COMPLETE\n";
    cout << "========================================\n";
}