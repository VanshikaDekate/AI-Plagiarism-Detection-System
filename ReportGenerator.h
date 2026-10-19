#ifndef REPORTGENERATOR_H
#define REPORTGENERATOR_H

#include "AIDetector.h"
#include "SimilarityAnalyzer.h"

#include <string>
#include <vector>

using namespace std;


/*
============================================================
CLASS: ReportGenerator
============================================================

RESPONSIBILITY:
    Display analysis results in a readable format.

DATA STRUCTURES USED:

    FeatureVector
    SimilarityResult
    vector<SimilarityResult>

No analysis algorithms are performed here.

This class is responsible only for presentation.
============================================================
*/

class ReportGenerator
{
public:

    void showAIReport(
        const FeatureVector& features,
        double score,
        const string& classification
    ) const;


    void showPlagiarismReport(
        const vector<SimilarityResult>& results
    ) const;


    void showCombinedReport(
        const FeatureVector& features,
        double aiScore,
        const string& classification,
        const vector<SimilarityResult>& results
    ) const;
};

#endif