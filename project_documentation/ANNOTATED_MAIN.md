# Annotated `main.cpp`

The source is [`main.cpp`](../main.cpp). Its `main` function is the orchestration layer; calculations belong to the helper classes described in their respective annotated files.

## Includes and entry

- Lines 1–6 include each module used: preprocessing, reference loading, AI feature scoring, similarity analysis, reports, and paraphrasing. `<iostream>` supplies console streams, `<string>` text, `<unordered_map>` word counts, `<vector>` sequences/results, and `<limits>` the `streamsize` bound used to discard the remainder of an input line.
- `using namespace std;` avoids qualifying standard-library names in this translation unit; it is a style convenience, not a requirement.
- `int main()` begins execution and returns an integer status to the operating system. Objects declared in it have automatic storage duration and are destroyed in reverse construction order on return.

## Input and preprocessing

1. The banner and prompts use chained `cout <<` operations. String literals containing `\n` explicitly add line breaks.
2. `originalText` owns the complete submitted text; `line` is a reusable local buffer. `getline(cin, line)` reads a full line (including spaces, excluding the newline). An exact line equal to `"END"` breaks the loop; otherwise `"\n"` is inserted between non-first lines so paragraph boundaries are retained. EOF also ends the loop. There is no empty-input early return in this version.
3. `TextProcessor processor` is reused for original and rephrased text. `cleanedText` receives normalized text; `words` keeps token order; `ngrams` holds all consecutive 3-word strings. The same vector is used for analysis and displayed trigram count.
4. `frequency` maps each token to its count. `frequency[word]++` inserts a missing key with value-initialized zero, then increments it. The range loop binds `word` as `const string&`, avoiding a copy per token.
5. `words.size()` and `ngrams.size()` are `size_t` values printed as counts. The n-gram count is `max(W-3+1, 0)` for W words.

## Original analysis

- `AIDetector aiDetector` initializes the fixed function-word list. `extractFeatures(originalText)` uses original text so punctuation and sentence delimiters remain available; it independently cleans and tokenizes inside the detector. `calculateScore` returns a 0–100 heuristic number; `classify` maps it to one of three strings.
- `DocumentManager documentManager` loads the hard-coded relative path `"data/reference"`. The path is resolved from the process's current working directory, not from the executable's location. Each accepted file becomes an indexed `Document`.
- `const vector<Document>& documents` aliases the manager-owned vector. It avoids copying all documents; the reference remains valid while the manager and its vector are not invalidated.
- `SimilarityAnalyzer analyzer` compares the submitted ordered words and trigrams with each document. The `frequency` argument is passed to `analyzeAll`, but the implementation explicitly suppresses its unused warning and constructs its own count maps in `cosine`.
- `ReportGenerator report` prints the AI and plagiarism reports. `showCombinedReport` delegates to two display methods and then prints a completion banner.

## Optional paraphrasing and second pass

- `char choice` stores one response character. `cin >> choice` skips leading whitespace and extracts one character. `cin.ignore(numeric_limits<streamsize>::max(), '\n')` discards the rest of that line. Only `y` or `Y` enters the branch; any other successfully read character declines.
- `Paraphraser paraphraser` constructs fixed maps. `rephrase(originalText)` applies phrase substitutions, then individual substitutions, and sentence-start capitalization. The resulting `rephrasedText` is printed.
- A second response controls re-analysis. If accepted, `rephrasedCleanedText`, `rephrasedWords`, `rephrasedNgrams`, and `rephrasedFrequency` mirror the original preprocessing. Recalculation then produces `rephrasedFeatures`, `rephrasedAiScore`, `rephrasedClassification`, and `rephrasedResults`; the same references and reusable service objects are used.
- The before/after AI difference is `rephrasedAiScore - aiScore`, in score points because both scores are already on a 0–100 scale.
- `originalSimilarity` and `rephrasedSimilarity` are initialized to zero and replaced with each sorted vector's first `finalScore` if a result exists. These values are fractions in [0,1], but this `main` prints them followed by `%` without multiplying by 100. The report itself correctly multiplies similarity values by 100; this comparison output is a verified unit/formatting bug.
- The closing comments say the rephraser is not yet called and Part 2 will add before/after analysis, but that description is stale: both behaviors are implemented above.
- `return 0` indicates normal completion. Errors such as missing corpus directory are reported by `DocumentManager`, but execution continues with an empty reference collection.

## Data flow summary

`originalText` is the canonical source. `TextProcessor` converts it into `words` and `ngrams`; `frequency` is built in `main`. `AIDetector` receives the original characters. `DocumentManager` supplies preprocessed document data. `SimilarityAnalyzer` returns sorted `SimilarityResult` objects. `ReportGenerator` only formats those values. The optional rephrased string is fed through the same stages, producing separate result variables rather than overwriting original results.

## Input and lifetime notes

The input loop will process what it collected if EOF arrives without `END`. An empty input or input containing no alphabetic tokens is not stopped here: feature extraction returns default-zero fields, the score calculation still awards its non-repetition component, similarity results are generally zero, and the user is still prompted. If extraction of `choice` fails at EOF, the uninitialized `choice` may be read; see the findings report.
