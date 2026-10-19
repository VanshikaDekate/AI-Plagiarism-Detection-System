#include "AIDetector.h"
#include "Document.h"
#include "DocumentManager.h"
#include "Paraphraser.h"
#include "ReportGenerator.h"
#include "SimilarityAnalyzer.h"
#include "TextProcessor.h"

#include <cassert>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace
{

class StreamCapture
{
public:
    explicit StreamCapture(std::ostream& stream)
        : buffer_(), stream_(stream), previous_(stream.rdbuf(buffer_.rdbuf()))
    {
    }

    ~StreamCapture()
    {
        stream_.rdbuf(previous_);
    }

    std::string str() const
    {
        return buffer_.str();
    }

private:
    std::stringstream buffer_;
    std::ostream& stream_;
    std::streambuf* previous_;
};

bool approximatelyEqual(double left, double right)
{
    return std::fabs(left - right) < 1e-12;
}

void testTextProcessor()
{
    TextProcessor processor;

    assert(processor.cleanText("").empty());
    assert(processor.cleanText(" \t\r\n").empty());
    assert(processor.tokenize(processor.cleanText("...!?")).empty());
    assert(processor.cleanText("  HELLO,\tWORLD!  ")
           == "hello world!");

    const std::vector<std::string> words =
        processor.tokenize("one two three.");
    assert((words == std::vector<std::string>{"one", "two", "three"}));

    assert(processor.generateNGrams(words, -1).empty());
    assert(processor.generateNGrams(words, 0).empty());
    assert(processor.generateNGrams({"one", "two"}, 3).empty());
    assert((processor.generateNGrams(words, 3) ==
            std::vector<std::string>{"one two three"}));
    assert((processor.generateNGrams(words, 2) ==
            std::vector<std::string>{"one two", "two three"}));

    std::vector<std::string> longWords(4096, "word");
    const auto longNgrams = processor.generateNGrams(longWords, 3);
    assert(longNgrams.size() == longWords.size() - 2);
    assert(longNgrams.front() == "word word word");
    assert(longNgrams.back() == "word word word");
}

void testDocumentIndex()
{
    Document document("sample.txt", "One two one three.");
    assert((document.words ==
            std::vector<std::string>{"one", "two", "one", "three"}));
    assert(document.frequency.at("one") == 2);
    assert(document.frequency.at("two") == 1);
    assert(document.ngrams.count("one two one") == 1);
    assert(document.ngrams.count("two one three") == 1);

    document.content = "replacement content";
    document.buildIndex();
    assert(document.frequency.count("one") == 0);
    assert(document.frequency.at("replacement") == 1);
    assert(document.ngrams.empty());
}

void testDocumentManager()
{
    namespace fs = std::filesystem;
    const fs::path temporary =
        fs::temp_directory_path() /
        ("ai_plagiarism_system_core_tests_" +
         std::to_string(std::chrono::steady_clock::now()
                            .time_since_epoch()
                            .count()));
    std::error_code error;
    fs::create_directories(temporary, error);
    assert(!error);

    const fs::path emptyPath = temporary / "empty.txt";
    const fs::path validPath = temporary / "valid.txt";
    const fs::path unreadablePath = temporary / "unreadable.txt";
    const fs::path notDirectoryPath = temporary / "not-a-directory";

    {
        std::ofstream emptyFile(emptyPath);
        std::ofstream validFile(validPath);
        std::ofstream unreadableFile(unreadablePath);
        std::ofstream notDirectoryFile(notDirectoryPath);
        assert(emptyFile && validFile && unreadableFile && notDirectoryFile);
        validFile << "alpha beta gamma";
        unreadableFile << "hidden content";
        notDirectoryFile << "not a directory";
    }

    DocumentManager manager;

    {
        StreamCapture capturedErrors(std::cerr);
        assert(manager.readFile((temporary / "missing.txt").string()).empty());
        assert(!capturedErrors.str().empty());
    }

    {
        StreamCapture capturedOutput(std::cout);
        StreamCapture capturedErrors(std::cerr);
        manager.loadDocuments((temporary / "missing-directory").string());
        assert(manager.getDocuments().empty());
        assert(capturedOutput.str().empty());
        assert(capturedErrors.str().find("does not exist")
               != std::string::npos);
    }

    {
        StreamCapture capturedErrors(std::cerr);
        manager.loadDocuments(notDirectoryPath.string());
        assert(manager.getDocuments().empty());
        assert(!capturedErrors.str().empty());
    }

    fs::permissions(unreadablePath, fs::perms::none,
                    fs::perm_options::replace, error);
    assert(!error);
    {
        StreamCapture capturedErrors(std::cerr);
        assert(manager.readFile(unreadablePath.string()).empty());
        assert(!capturedErrors.str().empty());
    }
    {
        StreamCapture capturedOutput(std::cout);
        StreamCapture capturedErrors(std::cerr);
        manager.loadDocuments(temporary.string());
        assert(manager.getDocuments().size() == 1);
        assert(manager.getDocuments().front().filename == "valid.txt");
        assert(capturedOutput.str().find("Ignoring empty reference file")
               != std::string::npos);
        assert(capturedErrors.str().find("Could not open file")
               != std::string::npos);
    }

    fs::permissions(unreadablePath,
                    fs::perms::owner_read | fs::perms::owner_write,
                    fs::perm_options::replace, error);
    assert(!error);

    fs::remove_all(temporary, error);
    assert(!error);
}

void testAIDetector()
{
    AIDetector detector;
    const FeatureVector emptyFeatures = detector.extractFeatures("");
    const FeatureVector whitespaceFeatures =
        detector.extractFeatures(" \t\r\n");
    const FeatureVector punctuationFeatures =
        detector.extractFeatures("...!?");

    assert(detector.calculateScore(emptyFeatures) == 0.0);
    assert(detector.calculateScore(whitespaceFeatures) == 0.0);
    assert(detector.calculateScore(punctuationFeatures) == 0.0);
    assert(detector.classify(0.0) == "Lower AI-like pattern");

    const FeatureVector shortFeatures =
        detector.extractFeatures("A cat.");
    assert(shortFeatures.avgSentenceLength == 2.0);
    assert(shortFeatures.avgWordLength == 2.0);

    const FeatureVector longFeatures =
        detector.extractFeatures(std::string(10000, 'a') + " word");
    assert(std::isfinite(longFeatures.avgWordLength));
    assert(longFeatures.avgWordLength > 0.0);

    FeatureVector maximal;
    maximal.avgSentenceLength = 1000.0;
    maximal.sentenceLengthVariance = 1000.0;
    maximal.avgWordLength = 1000.0;
    maximal.vocabularyRichness = 1.0;
    maximal.repetitionRatio = 0.0;
    maximal.punctuationRate = 1.0;
    maximal.functionWordRate = 1.0;
    assert(detector.calculateScore(maximal) == 100.0);

    maximal.avgSentenceLength = -100.0;
    maximal.sentenceLengthVariance = -100.0;
    maximal.avgWordLength = -100.0;
    maximal.vocabularyRichness = -1.0;
    maximal.punctuationRate = -1.0;
    maximal.functionWordRate = -1.0;
    assert(detector.calculateScore(maximal) >= 0.0);

    FeatureVector invalid;
    invalid.avgWordLength = std::numeric_limits<double>::infinity();
    assert(detector.calculateScore(invalid) == 0.0);
    invalid.avgWordLength = std::numeric_limits<double>::quiet_NaN();
    assert(detector.calculateScore(invalid) == 0.0);
}

void testSimilarityAnalyzer()
{
    SimilarityAnalyzer analyzer;
    const std::vector<std::string> empty;
    const std::vector<std::string> alpha{"alpha", "beta", "alpha"};
    const std::vector<std::string> identical{"alpha", "beta", "alpha"};
    const std::vector<std::string> unrelated{"gamma", "delta"};
    const std::vector<std::string> partial{"alpha", "gamma"};

    assert(analyzer.jaccard(empty, empty) == 0.0);
    assert(analyzer.cosine(empty, alpha) == 0.0);
    assert(analyzer.jaccard(alpha, identical) == 1.0);
    assert(approximatelyEqual(analyzer.cosine(alpha, identical), 1.0));
    assert(analyzer.jaccard(alpha, unrelated) == 0.0);
    assert(analyzer.cosine(alpha, unrelated) == 0.0);
    assert(analyzer.jaccard(alpha, partial) > 0.0);
    assert(analyzer.cosine(alpha, partial) > 0.0);

    assert(analyzer.ngramSimilarity(empty, empty) == 0.0);
    assert(analyzer.ngramSimilarity({"a b c"}, empty) == 0.0);
    assert(analyzer.ngramSimilarity({"a b c"}, {"a b c"}) == 1.0);
    assert(approximatelyEqual(
        analyzer.ngramSimilarity(
            {"a b c", "a b c", "b c d"},
            {"a b c", "x y z"}),
        1.0 / 3.0));

    const SimilarityResult analyzed = analyzer.analyze(
        alpha, {"a b c", "a b c"}, "same.txt",
        identical, {"a b c"});
    assert(analyzed.filename == "same.txt");
    assert(analyzed.jaccard == 1.0);
    assert(approximatelyEqual(analyzed.cosine, 1.0));
    assert(analyzed.ngram == 1.0);
    assert(approximatelyEqual(analyzed.finalScore, 1.0));
    assert(analyzed.matchingPhrases.size() == 1);

    std::vector<Document> documents;
    documents.emplace_back("unrelated.txt", "gamma delta");
    documents.emplace_back("partial.txt", "alpha gamma");
    documents.emplace_back("identical.txt", "alpha beta alpha");
    std::unordered_map<std::string, int> frequencies{
        {"alpha", 2}, {"beta", 1}};
    const auto results =
        analyzer.analyzeAll(alpha, frequencies, {}, documents);
    assert(results.size() == 3);
    assert(results.front().filename == "identical.txt");
    for (const SimilarityResult& result : results)
    {
        assert(result.jaccard >= 0.0 && result.jaccard <= 1.0);
        assert(result.cosine >= 0.0 && result.cosine <= 1.0);
        assert(result.ngram >= 0.0 && result.ngram <= 1.0);
        assert(result.finalScore >= 0.0 && result.finalScore <= 1.0);
    }
    for (std::size_t i = 1; i < results.size(); ++i)
    {
        assert(results[i - 1].finalScore >= results[i].finalScore);
    }
    assert(analyzer.analyzeAll(alpha, frequencies, {}, {}).empty());

    std::vector<Document> tiedDocuments;
    for (int i = 0; i < 1000; ++i)
    {
        tiedDocuments.emplace_back("tie-" + std::to_string(i),
                                   "same words here");
    }
    const auto tiedResults =
        analyzer.analyzeAll({"same", "words", "here"}, {},
                            {"same words here"}, tiedDocuments);
    assert(tiedResults.size() == tiedDocuments.size());
    for (std::size_t i = 1; i < tiedResults.size(); ++i)
    {
        assert(tiedResults[i - 1].finalScore >= tiedResults[i].finalScore);
    }
}

void testParaphraserAndReports()
{
    Paraphraser paraphraser;
    assert(paraphraser.rephrase("") == "");
    assert(paraphraser.rephrase(
               "Due to the fact that the result is large, it matters!")
           == "Because the outcome is substantial, it matters!");
    assert(paraphraser.rephrase("important important.")
           == "Significant significant.");
    assert(paraphraser.rephrase("The methodology is important.")
           == "The methodology is significant.");
    assert(paraphraser.rephrase(
               "AI, C++, ESP32, and 2026 are technical terms.")
           == "AI, C++, ESP32, and 2026 are technical terms.");
    assert(paraphraser.rephrase("one. two! three? four.")
           == "One. Two! Three? Four.");

    ReportGenerator report;
    FeatureVector features;
    std::vector<SimilarityResult> noResults;
    {
        StreamCapture captured(std::cout);
        report.showCombinedReport(features, 0.0,
                                  "Lower AI-like pattern", noResults);
        const std::string output = captured.str();
        assert(output.find("0.00%") != std::string::npos);
        assert(output.find("LOCAL CORPUS SIMILARITY REPORT")
               != std::string::npos);
        assert(output.find("No reference documents found.")
               != std::string::npos);
        assert(output.find("does not prove authorship")
               != std::string::npos);
        assert(output.find("not proof of plagiarism or originality")
               != std::string::npos);
    }

    SimilarityResult result;
    result.filename = "partial.txt";
    result.jaccard = 0.25;
    result.cosine = 0.5;
    result.ngram = 0.0;
    result.finalScore = 0.325;
    {
        StreamCapture captured(std::cout);
        report.showPlagiarismReport({result});
        assert(captured.str().find("32.50%") != std::string::npos);
    }
}

} // namespace

int main()
{
    testTextProcessor();
    testDocumentIndex();
    testDocumentManager();
    testAIDetector();
    testSimilarityAnalyzer();
    testParaphraserAndReports();
    std::cout << "All core tests passed.\n";
}
