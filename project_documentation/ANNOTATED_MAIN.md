# Annotated `main.cpp`

The source is [`main.cpp`](../main.cpp). Its `main` function is the orchestration layer; calculations belong to the helper classes described in their respective annotated files.

## Includes and entry

- Lines 1–6 include each module used: preprocessing, reference loading, AI feature scoring, similarity analysis, reports, and paraphrasing. `<iostream>` supplies console streams, `<string>` text, `<unordered_map>` word counts, `<vector>` sequences/results, and `<cstdlib>` the `EXIT_FAILURE` status used for invalid input.
- `using namespace std;` avoids qualifying standard-library names in this translation unit; it is a style convenience, not a requirement.
- `int main()` begins execution and returns an integer status to the operating system. Objects declared in it have automatic storage duration and are destroyed in reverse construction order on return.

## Input and preprocessing

1. The banner and prompts use chained `cout <<` operations. String literals containing `\n` explicitly add line breaks.
2. `originalText` owns the complete submitted text; `line` is a reusable local buffer. `getline(cin, line)` reads a full line (including spaces, excluding the newline). An exact line equal to `"END"` breaks the loop; otherwise `"\n"` is inserted between non-first lines so paragraph boundaries are retained. EOF also ends collection and the collected text is analyzed. A bad input stream is reported and exits with failure. If tokenization yields no words, `main` reports invalid text and exits with `EXIT_FAILURE`.
3. `TextProcessor processor` is reused for original and rephrased text. `cleanedText` receives normalized text; `words` keeps token order; `ngrams` holds all consecutive 3-word strings. The same vector is used for analysis and displayed trigram count.
4. `frequency` maps each token to its count. `frequency[word]++` inserts a missing key with value-initialized zero, then increments it. The range loop binds `word` as `const string&`, avoiding a copy per token.
5. `words.size()` and `ngrams.size()` are `size_t` values printed as counts. The n-gram count is `max(W-3+1, 0)` for W words.

## Original analysis

- `AIDetector aiDetector` initializes the fixed function-word list. `extractFeatures(originalText)` uses original text so punctuation and sentence delimiters remain available; it independently cleans and tokenizes inside the detector. `calculateScore` returns a 0–100 heuristic number; `classify` maps it to one of three strings.
- `DocumentManager documentManager` loads the hard-coded relative path `"data/reference"`. The path is resolved from the process's current working directory, not from the executable's location. Each accepted file becomes an indexed `Document`.
- `const vector<Document>& documents` aliases the manager-owned vector. It avoids copying all documents; the reference remains valid while the manager and its vector are not invalidated.
- `SimilarityAnalyzer analyzer` compares the submitted ordered words and n-grams with each document. The `frequency` argument is passed to `analyzeAll`, but is explicitly unused; `cosine` constructs its own count maps.
- `ReportGenerator report` prints the heuristic and local-corpus similarity reports. `showCombinedReport` delegates to two display methods and then prints a completion banner.

## Optional paraphrasing and second pass

- `askYesNo(const string& prompt, bool& answer)` reads a complete line, trims surrounding spaces/tabs/CR, accepts only `y/Y/n/N`, and reprompts invalid responses. EOF returns `false` to the caller; the caller exits safely. The output booleans `rephraseChoice` and `analyzeRephrased` are initialized before calling the helper and are distinct, so there is no variable shadowing.
- `Paraphraser paraphraser` constructs fixed maps. `rephrase(originalText)` applies phrase substitutions, then individual substitutions, and sentence-start capitalization. The resulting `rephrasedText` is printed.
- A second response controls re-analysis. If accepted, `rephrasedCleanedText`, `rephrasedWords`, `rephrasedNgrams`, and `rephrasedFrequency` mirror the original preprocessing. Recalculation then produces `rephrasedFeatures`, `rephrasedAiScore`, `rephrasedClassification`, and `rephrasedResults`; the same references and reusable service objects are used.
- The before/after AI difference is `rephrasedAiScore - aiScore`, in score points because both scores are already on a 0–100 scale.
- `originalSimilarity` and `rephrasedSimilarity` are initialized to zero and replaced with the best `finalScore * 100.0` if a result exists. The displayed change is therefore in percentage points, consistent with the `%` suffix.
- Closing comments describe the optional rephrasing and second analysis that are implemented above.
- `return 0` indicates normal completion. Errors such as missing corpus directory are reported by `DocumentManager`, but execution continues with an empty reference collection.

## Data flow summary

`originalText` is the canonical source. `TextProcessor` converts it into `words` and `ngrams`; `frequency` is built in `main`. `AIDetector` receives the original characters. `DocumentManager` supplies preprocessed document data. `SimilarityAnalyzer` returns sorted `SimilarityResult` objects. `ReportGenerator` only formats those values. The optional rephrased string is fed through the same stages, producing separate result variables rather than overwriting original results.

## Input and lifetime notes

The input loop processes collected text if EOF arrives without `END`. Empty, whitespace-only, and punctuation-only submissions produce no tokens and are rejected before analysis. EOF at either yes/no prompt exits cleanly. The exact sentinel is still required to be a whole line equal to `"END"`.
