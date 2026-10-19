# Variable reference

This inventories named parameters, members, locals, and loop variables in the inspected C++ files. Standard-library function internals are not project variables. Unless noted, locals live until their function returns; class members live with their owning object. `const T&` is a non-owning read-only alias; ordinary `T` owns its value. Vectors/maps/strings are copied when passed or assigned by value and referenced where the signature explicitly uses `&`.

## `main.cpp`

| Name | Type / scope | Initial value and role | Reads / modifications / ownership |
|---|---|---|---|
| `originalText` | `string`, `main` | Empty; owns entered paragraph. | Appends line breaks and each non-`END` line; read by preprocessing, AI analysis, paraphrasing. |
| `line` | `string`, input loop | Empty, then overwritten by `getline`; line buffer. | Compared with `"END"` and appended to `originalText`. |
| `processor` | `TextProcessor`, `main` | Default-constructed stateless service. | Read-only method calls for original/rephrased text. |
| `cleanedText` | `string`, `main` | Result of cleaning `originalText`. | Read by `tokenize`; owns normalized copy. |
| `words` | `vector<string>`, `main` | Tokens from cleaned text. | Iterated for count map; passed by const reference to analysis. |
| `ngrams` | `vector<string>`, `main` | Trigrams from `words`. | Displayed/count and passed to analyzer. |
| `frequency` | `unordered_map<string,int>`, `main` | Empty then built by `frequency[word]++`. | Passed to `analyzeAll`, where currently ignored; owns counts. |
| `word` | `const string&`, frequency loop | Alias of each token. | Read to increment map; no copy or mutation. |
| `aiDetector` | `AIDetector`, `main` | Initializes dictionary. | Called to extract, score, classify original and rephrased text. |
| `features` | `FeatureVector`, `main` | Result from original text. | Read by scoring/report; aggregate fields owned by value. |
| `aiScore` | `double`, `main` | Original score, [nominally] 0–100. | Printed and subtracted from rephrased score. |
| `aiClassification` | `string`, `main` | Threshold label. | Printed in report. |
| `documentManager` | `DocumentManager`, `main` | Empty collection, then loads path. | Owns documents for duration of analysis. |
| `documents` | `const vector<Document>&`, `main` | Alias returned from manager. | Read by `analyzeAll`; not copied or modified here. |
| `analyzer` | `SimilarityAnalyzer`, `main` | Stateless service. | Computes sorted original/rephrased results. |
| `results` | `vector<SimilarityResult>`, `main` | Original corpus comparison. | Reported and `front()` read for best score if nonempty. |
| `report` | `ReportGenerator`, `main` | Stateless presentation object. | Prints original and optional rephrased results. |
| `rephraseChoice` | `bool`, `main` | Initialized false; output parameter set by `askYesNo` after a valid response. | Controls whether the first optional branch runs. |
| `paraphraser` | `Paraphraser`, optional branch | Builds fixed phrase/synonym maps. | Read through `rephrase`. |
| `rephrasedText` | `string`, optional branch | Output of `rephrase(originalText)`. | Printed and conditionally analyzed. |
| `rephrasedCleanedText` | `string`, second-pass branch | Cleaned rephrased text. | Tokenized. |
| `rephrasedWords` | `vector<string>`, second pass | Rephrased tokens. | Used for counts and similarity. |
| `rephrasedNgrams` | `vector<string>`, second pass | Rephrased trigrams. | Passed to analyzer. |
| `analyzeRephrased` | `bool`, second pass | Initialized false; set by `askYesNo` after a valid response. | Controls the second analysis branch; distinct from `rephraseChoice`. |
| `rephrasedFrequency` | `unordered_map<string,int>`, second pass | Empty, then populated by word loop. | Passed but ignored by `analyzeAll`. |
| `rephrasedFeatures` | `FeatureVector`, second pass | Features for rephrased text. | Scored and reported. |
| `rephrasedAiScore` | `double`, second pass | Score for rephrased text. | Printed and compared with `aiScore`. |
| `rephrasedClassification` | `string`, second pass | Threshold class. | Printed by report. |
| `rephrasedResults` | `vector<SimilarityResult>`, second pass | Sorted rephrased comparison. | Reported and inspected for best score. |
| `originalSimilarity` | `double`, comparison block | 0.0 fallback; overwritten with the first original result's `finalScore * 100`. | Printed as a percentage. |
| `rephrasedSimilarity` | `double`, comparison block | Same for rephrased result. | Printed as a percentage. |

## `TextProcessor.cpp`

| Name | Type / scope | Role and mutation |
|---|---|---|
| `text` | `const string&`, method parameter | Borrowed input; never modified. |
| `cleaned` | `string`, `cleanText` | Owns per-character normalized text; appended/reserved. |
| `ch` | `char`, range loop / `stringstream::get` | Character copied from input or read from stream; tested and appended, not used to modify source. |
| `ss` | `stringstream`, normalization/tokenizer | Reads `cleaned` or `text` as formatted tokens; in AI feature code separate stream. |
| `word` | `string`, stream-loop local | Reused extraction buffer; modified by `pop_back` in tokenizer. |
| `normalized` | `string`, `cleanText` | Owns collapsed-whitespace result; appended and returned. |
| `words` | `vector<string>`, `tokenize` | Owns ordered tokens; appended to and returned. |
| `words` | `const vector<string>&`, `generateNGrams` | Borrowed source sequence, read-only. |
| `n` | `int`, `generateNGrams` | Requested window width; validated nonpositive/too-large values. |
| `ngrams` | `vector<string>`, `generateNGrams` | Owns generated grams, appended and returned. |
| `i` | `size_t`, outer gram loop | Window start index; incremented across valid starts. |
| `gram` | `string`, inner loop | Current output window; constructed then copied/moved into `ngrams`. |
| `j` | `int`, inner gram loop | Word offset within the window. |

## `Document` and `DocumentManager`

| Name | Type / scope | Role and mutation |
|---|---|---|
| `filename`, `content` | `string`, public `Document` members | Own basename and raw source text. Constructor assigns; content is read during indexing. |
| `words` | `vector<string>`, public member | Owns ordered cleaned tokens. Set in `buildIndex`. |
| `frequency` | `unordered_map<string,int>`, public member | Owns counts; cleared and incremented in `buildIndex`. |
| `ngrams` | `unordered_set<string>`, public member | Owns distinct trigrams; cleared and inserted in `buildIndex`. |
| `filename`, `content` | `const string&`, constructor parameters | Borrowed input copied into corresponding members. |
| `this` | `Document*` implicit parameter | Points to the constructed object; `this->` disambiguates member assignments. |
| `processor` | `TextProcessor`, `buildIndex` | Stateless local preprocessing service. |
| `cleaned` | `string`, `buildIndex` | Owns cleaned content until indexing is done. |
| `generatedNGrams` | `vector<string>`, `buildIndex` | Temporary ordered grams used to populate unique set. |
| `directoryPath` | `const string&`, `loadDocuments` | Borrowed path string; passed to filesystem check/iterator. |
| `filename` | `const string&`, `readFile` | Borrowed file path; passed to `ifstream` and error output. |
| `file` | `ifstream`, `readFile` | Owns open file stream state. |
| `buffer` | `stringstream`, `readFile` | Accumulates complete stream buffer. |
| `entry` | `const auto&`, directory loop | Borrowed filesystem directory entry; inspected for type/extension/path. |
| `filename` | `string`, load loop | Basename stored in `Document`. |
| `fullPath` | `string`, load loop | Filesystem path used by `readFile`. |
| `content` | `string`, load loop | Owns file bytes until copied/indexed into `Document`. |
| `document` | `Document`, load loop | Local indexed document copied into `documents` by `push_back`. |
| `documents` | `vector<Document>`, manager member | Owns corpus; cleared/repopulated by load and aliased by getter. |

## `AIDetector.cpp`

`FeatureVector` declares seven public `double` data members, each initialized to `0.0`: `avgSentenceLength`, `sentenceLengthVariance`, `avgWordLength`, `vocabularyRichness`, `repetitionRatio`, `punctuationRate`, and `functionWordRate`. They live as part of each `FeatureVector` object and are written by `extractFeatures`, read by `calculateScore` and `ReportGenerator`; default initialization gives a safe zero state.

| Name | Type / scope | Role and mutation |
|---|---|---|
| `functionWords` | `vector<string>`, detector member | Fixed dictionary initialized in constructor and read by searches. |
| `word` | `const string&`, `isFunctionWord` | Read-only query into function-word dictionary. |
| `text` | `const string&`, `extractFeatures` | Borrowed original text. |
| `features` | `FeatureVector`, `extractFeatures` | Local result; fields updated as features are computed. |
| `processor` | `TextProcessor`, `extractFeatures` | Local stateless helper. |
| `cleaned` | `string` | Owns normalized text. |
| `words` | `vector<string>` | Owns tokenized words. |
| `frequency` | `unordered_map<string,int>` | Owns per-word counts. |
| `word` | `const string&`, loops | Read-only alias to current token. |
| `uniqueWords` | `unordered_set<string>` | Owns distinct vocabulary. |
| `totalCharacters` | `double` | Sum of token lengths, then average numerator. |
| `pair` | `const auto&` | Alias to map key/value; `pair.second` is count. |
| `repeatedWords` | `size_t` | Counts extra occurrences (count minus one). |
| `functionWordCount` | `size_t` | Counts tokens in fixed dictionary. |
| `punctuationCount` | `size_t` | Counts punctuation bytes in original input. |
| `ch` | `char` | Current original character in punctuation/sentence loops. |
| `sentenceLengths` | `vector<size_t>` | Owns nonempty sentence word counts without narrowing. |
| `currentSentence` | `string` | Accumulates characters until a sentence terminator; cleared on boundary. |
| `ss` | `stringstream` | Reads original text one character at a time. |
| `sentenceWords` | `vector<string>` | Temporary tokens for each sentence; size appended when nonempty. |
| `sum`, `mean`, `variance` | `double` | Accumulator, average sentence length, and population variance. |
| `length` | `int` | Current sentence's word count during variance loop. |
| `f` | `const FeatureVector&`, `calculateScore` | Read-only feature input. |
| `sentenceLengthScore`, `varianceScore`, `wordLengthScore`, `vocabularyScore`, `repetitionScore`, `punctuationScore`, `functionWordScore` | `double`, `calculateScore` | Upper-capped normalized values used in weighted sum. |
| `score` | `double`, score/classification scopes | In `calculateScore`, weighted result (local); in `classify`, input parameter. |

## `SimilarityAnalyzer.cpp` and report variables

| Name | Type / scope | Role and mutation |
|---|---|---|
| `filename` | `string`, `SimilarityResult` field / `const string&` analyze parameter | Result owns a copied filename; parameter is borrowed. |
| `jaccard`, `cosine`, `ngram`, `finalScore` | `double`, result fields | Component values and weighted fractional score. |
| `matchingPhrases` | `vector<string>`, result field | Owns matching submitted trigram copies. |
| `words1`, `words2`, `ngrams1`, `ngrams2` | `const vector<string>&` metric parameters | Borrowed read-only sequences. |
| `set1`, `set2`, `referenceNgrams` | `unordered_set<string>` locals | Own unique strings for membership testing. |
| `intersection`, `unionSize`, `matches` | `int` locals | Accumulate set overlap/counts; integer-sized. |
| `freq1`, `freq2` | `unordered_map<string,int>` | Own token counts for cosine. |
| `word` | `const string&` | Alias of current token/set item. |
| `dotProduct`, `magnitude1`, `magnitude2` | `double` | Dot product and squared norms. |
| `item` | `const auto&` | Alias to map pair. |
| `count1` | `int` | First vector's count for current shared term. |
| `submittedWords`, `submittedNgrams`, `documentWords`, `documentNgrams` | const-reference parameters | Borrowed sequences read by `analyze`. |
| `filename` | `const string&`, `analyze` | Borrowed source name, copied into result. |
| `result` | `SimilarityResult`, `analyze` / `analyzeAll` | Mutable local result populated then returned or appended. |
| `low`, `high`, `nextGreater`, `j`, `pivotIndex` | `size_t`, Quick Sort | Nonnegative partition bounds, scan position, and pivot index. |
| `pivot` | `double` | Last result's score for current partition. |
| `frequency` | `const unordered_map<string,int>&`, `analyzeAll` | Borrowed but explicitly unused compatibility parameter. |
| `documents` | `const vector<Document>&` | Borrowed corpus. |
| `document` | `const Document&`, corpus loop | Read-only alias to one reference. |
| `documentNgrams` | `vector<string>` | Temporary copy from unordered document trigram set. |
| `results` | `vector<SimilarityResult>` | Owns all per-document results; sorted in place. |

`ReportGenerator` parameters `features`, `results`, `classification` are const references; `score` and `aiScore` are value `double`s. Its locals: `rank` starts at one and increments; `best` aliases the first result; `result` aliases each ranked entry; `count` increments while printing; `phrase` aliases each matching phrase. They are all read-only except the counters. The added `askYesNo` helper has a `const string& prompt`, a `bool& answer` set only on valid input, and local `input`, `first`, and `last` values for trimming and validation.

## `Paraphraser.cpp` and test drivers

| Name | Type / scope | Role and mutation |
|---|---|---|
| `phraseMap` | `unordered_map<string,string>`, member | Owns phrase-rule keys and replacements. |
| `synonyms` | `unordered_map<string,vector<string>>`, member | Owns word-rule keys and replacement candidates. |
| `text` | `const string&`, replacement/rephrase methods | Borrowed input. |
| `result` | `string`, replacement/rephrase methods | Mutable owned transformed copy. |
| `entry` | `const auto&`, map loops | Read-only map key/value alias. |
| `pattern` | `const string`, rule loop | Owns generated regex source. |
| `expression` | `const regex`, rule loop | Compiled case-insensitive expression. |
| `capitalizeNext` | `bool`, `rephrase` | State flag controlling capitalization. |
| `ch` | `char&`, final scan | Mutable alias to each character in `result`. |
| `uch` | `unsigned char` | Safe byte value passed to `<cctype>` functions. |
| `paraphraser` | `Paraphraser`, test `main` | Owns rules for the smoke test. |
| `tests` | `vector<string>`, test `main` | Owns six fixed sample strings. |
| `i` | `size_t`, test loop | Index into test cases; unsigned size type matches vector indexing. |

`tempCodeRunnerFile.cpp` has another `main` and repeats the production pipeline's local names for input, tokens, features, documents, analyzer, results, and report. It is a duplicate/temporary program and should not be compiled alongside `main.cpp`; its separate source does not provide a distinct production variable model.
