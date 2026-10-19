# Function reference

Complexities below describe the function's own work and explicitly identify important called-function costs. `N` is text/file character count; `W` token count; `U` unique-token count; `R` reference-document count. Hash-table bounds are average/expected unless noted.

## Program entry points

| Signature | Behavior, contract, dependencies, complexity |
|---|---|
| `int main()` in [`main.cpp`](../main.cpp) | Reads until exact `END` or EOF; preprocesses; counts; calls feature/scoring/classification, loads corpus, analyzes and reports; optionally rephrases and repeats. I/O and service costs dominate; no single useful complexity because input and corpus sizes vary. Returns 0 on normal completion. |
| `int main()` in [`test_paraphraser.cpp`](../test_paraphraser.cpp) | Runs six hard-coded paraphraser samples and prints results. Calls `Paraphraser::rephrase`; not an automated assertion-based test. |
| `int main()` in [`tempCodeRunnerFile.cpp`](../tempCodeRunnerFile.cpp) | Temporary duplicate analysis pipeline. Has a separate entry point and is not linked with production. |

## Text processing

| Signature | Preconditions / effects / complexity |
|---|---|
| `string TextProcessor::cleanText(const string& text)` | No precondition; returns lowercase ASCII/locale alphabetic bytes, one-space-normalized text, retaining `.?!` and replacing other characters with spaces. O(N) time and O(N) result/stream storage. |
| `vector<string> TextProcessor::tokenize(const string& text)` | Reads whitespace-separated tokens and strips trailing `.?!`; returns remaining nonempty tokens in order. O(N) time, O(W) token/container storage (plus token character storage). |
| `vector<string> TextProcessor::generateNGrams(const vector<string>& words, int n)` | Returns empty for `n<=0` or too few words; otherwise joins each consecutive n-word window. O((W-n+1)n) word operations; for fixed n=3, O(W), excluding copied characters in output. |

## Reference documents

| Signature | Preconditions / effects / complexity |
|---|---|
| `Document::Document()` | Default-constructs empty members; does not index. Constant apart from member construction. |
| `Document::Document(const string& filename, const string& content)` | Copies filename/content, then calls `buildIndex`. O(N+W) expected for fixed trigram width plus allocations. |
| `void Document::buildIndex()` | Replaces member indexes based on `content`: clean/tokenize/count/generate unique trigrams. O(N+W) expected for fixed trigram width, O(N+W) storage. |
| `string DocumentManager::readFile(const string& filename) const` | Opens and reads entire file; logs and returns empty on open failure. O(N) time/storage. |
| `void DocumentManager::loadDocuments(const string& directoryPath)` | Clears old state, iterates immediate directory entries, reads nonempty `.txt` files, creates indexed documents, and prints count. O(E + total content/indexing work) expected, E entries; O(total corpus storage). Filesystem exceptions are not caught. |
| `const vector<Document>& DocumentManager::getDocuments() const` | Returns a read-only alias to member collection. O(1); reference lifetime is tied to manager. |

## AI feature extraction

| Signature | Preconditions / effects / complexity |
|---|---|
| `AIDetector::AIDetector()` | Initializes the 27 fixed function words. O(F) construction/storage. |
| `bool AIDetector::isFunctionWord(const string& word) const` | Linear search via `std::find`; returns membership. O(F), no additional meaningful storage. |
| `FeatureVector AIDetector::extractFeatures(const string& text) const` | Empty-token input returns zero features. Otherwise computes counts, vocabulary, word/sentence metrics and punctuation. O(N+WF) expected (F=27 fixed; effectively O(N)), O(N+W) auxiliary storage. Uses `TextProcessor`; not a statistical classifier. |
| `double AIDetector::calculateScore(const FeatureVector& features) const` | Returns zero for non-finite or all-nonpositive input; clamps normalized fields and final weighted score to [0,100]. O(1), O(1). |
| `string AIDetector::classify(double score) const` | Threshold categories at 40 and 70. O(1), returns an owned string. Callers should supply the bounded detector score. |

## Similarity and ranking

| Signature | Preconditions / effects / complexity |
|---|---|
| `double SimilarityAnalyzer::jaccard(const vector<string>& words1, const vector<string>& words2)` | Converts to unique sets and computes intersection/union; empty union returns zero. O(W1+W2) expected, O(U1+U2) storage. |
| `double SimilarityAnalyzer::cosine(const vector<string>& words1, const vector<string>& words2)` | Builds frequency vectors and cosine score; zero vector returns zero. O(W1+W2+U1+U2) expected; O(U1+U2) storage. |
| `double SimilarityAnalyzer::ngramSimilarity(const vector<string>& ngrams1, const vector<string>& ngrams2)` | Builds unique sets and returns symmetric Jaccard overlap; both-empty returns zero. O(N1+N2) expected, O(U1+U2) set storage. |
| `SimilarityResult SimilarityAnalyzer::analyze(const vector<string>& submittedWords, const vector<string>& submittedNgrams, const string& filename, const vector<string>& documentWords, const vector<string>& documentNgrams)` | Calculates three bounded metrics, weighted final score in [0,1], and distinct matching submitted grams. O(Wq+Wd+Nq+Nd) expected. |
| `size_t SimilarityAnalyzer::partition(vector<SimilarityResult>& results, size_t low, size_t high)` | In-place descending partition around last score; returns pivot index. O(high-low) time, O(1) auxiliary storage. |
| `void SimilarityAnalyzer::quickSort(vector<SimilarityResult>& results, size_t low, size_t high)` | In-place Quick Sort; average O(R log R), worst O(R²) comparisons. Recurses on smaller side, keeping stack O(log R). |
| `vector<SimilarityResult> SimilarityAnalyzer::analyzeAll(const vector<string>& submittedWords, const unordered_map<string,int>& frequency, const vector<string>& submittedNgrams, const vector<Document>& documents)` | Calls `analyze` per document, sorts results, returns them. `frequency` is unused. Expected total per-doc work plus O(R log R) sorting; worst-case sorting O(R²); result storage O(R) plus per-result matching phrases. |

## Reporting

| Signature | Behavior / complexity |
|---|---|
| `void ReportGenerator::showAIReport(const FeatureVector& features, double score, const string& classification) const` | Prints feature values and label; O(1) output fields. Changes persistent `cout` formatting state. |
| `void ReportGenerator::showPlagiarismReport(const vector<SimilarityResult>& results) const` | Prints all R rankings and details of the first result, or empty-state message. O(R+10) maximum phrase iteration. Requires pre-sorted vector for correct rank meaning. |
| `void ReportGenerator::showCombinedReport(const FeatureVector& features, double aiScore, const string& classification, const vector<SimilarityResult>& results) const` | Calls the two display functions and prints a footer; O(R) through ranking print. |

## Paraphrasing

| Signature | Behavior / complexity |
|---|---|
| `Paraphraser::Paraphraser()` | Populates five phrase rules and nine synonym keys. O(P+S) setup. |
| `string Paraphraser::replacePhrases(const string& text)` | Copies text and applies each phrase regex, case-insensitively. Repeated regex scans; no strict linear upper bound for `std::regex`. |
| `string Paraphraser::replaceWords(const string& text)` | Applies each whole-word synonym regex and chooses first replacement. Similar repeated-regex caveat. |
| `string Paraphraser::rephrase(const string& text)` | Empty input identity; otherwise calls phrase and word replacements and scans result for sentence capitalization. Complexity includes repeated regex work plus O(N) capitalization scan. Meaning and grammar preservation are not guaranteed. |
