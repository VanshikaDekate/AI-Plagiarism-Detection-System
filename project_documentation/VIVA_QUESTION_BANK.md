# University viva question bank

Answers below distinguish **verified implementation behavior** from general theory or recommendations. Current source is the authority; see [`PROJECT_OVERVIEW.md`](PROJECT_OVERVIEW.md) for the module map.

## Programming, C++, and object-oriented design

### 1. What is the role of `main()` in this project?

**Concise answer:** It coordinates input, preprocessing, AI-feature scoring, corpus loading, similarity analysis, report output, and optional paraphrasing/re-analysis.

**Deeper explanation:** `main` owns the run-specific values and constructs service objects. It delegates transformations and calculations to `TextProcessor`, `AIDetector`, `DocumentManager`, `SimilarityAnalyzer`, `ReportGenerator`, and `Paraphraser`; it does not implement all those algorithms itself.

**Reference:** [`main.cpp`](../main.cpp), `main`.

**Follow-up:** Which object owns the loaded reference documents? `DocumentManager` owns its `vector<Document>`; `main` holds a const reference to it.

### 2. Why use `const string&` for many input parameters?

**Concise answer:** It avoids copying an existing string while preventing the callee from modifying it.

**Deeper explanation:** A reference aliases the caller's object for the duration of the call. `const` enforces read-only access. Return values and stored members are values because the recipient must own the result. References must not outlive their owning object.

**Reference:** `TextProcessor` methods; `DocumentManager::getDocuments`.

**Follow-up:** Why is `getDocuments()` not returning a vector by value? A const reference avoids copying the entire corpus, but it is tied to the manager's lifetime.

### 3. What does a header guard do, and why are there forward declarations?

**Concise answer:** A header guard prevents a header's declarations from being processed more than once per translation unit. `SimilarityAnalyzer.h` forward-declares `Document` to reduce header coupling.

**Deeper explanation:** A class declaration may use an incomplete type in some contexts; its implementation includes `Document.h` for member access. `DocumentManager` stores `vector<Document>` in the header and includes its full definition.

**Reference:** [`TextProcessor.h`](../TextProcessor.h), [`SimilarityAnalyzer.h`](../SimilarityAnalyzer.h), [`DocumentManager.h`](../DocumentManager.h).

**Follow-up:** Why do headers include standard headers? To provide the declarations for types used in public signatures and members.

### 4. What happens when `frequency[word]++` sees a new word?

**Concise answer:** `unordered_map::operator[]` inserts the missing key with a value-initialized `int` (zero), then the postfix increment makes it one.

**Deeper explanation:** Hash maps give average constant-time key access, but worst-case collisions can degrade operations. The key and count are owned by the map.

**Reference:** `main`, `Document::buildIndex`, `SimilarityAnalyzer::cosine`.

**Follow-up:** Why not use a vector for counts? A map directly associates arbitrary token strings with counts; a vector needs a separate vocabulary-to-index mapping.

## Data structures and algorithms

### 5. Why does the program use both a vector, map, and set for text?

**Concise answer:** The vector preserves word order and duplicates, the map stores counts, and the set stores unique items for membership and overlap.

**Deeper explanation:** N-grams require adjacent ordered words, so a set alone loses essential order. Jaccard requires unique membership. Cosine requires frequencies. Document trigrams are deduplicated for reference membership.

**Reference:** [`Document.h`](../Document.h), [`SimilarityAnalyzer.cpp`](../SimilarityAnalyzer.cpp).

**Follow-up:** What is the main trade-off of unordered containers? Average O(1) lookup, but extra hash storage and O(n) worst-case operations; iteration order is unspecified.

### 6. What is the complexity of `generateNGrams`?

**Concise answer:** For W words and width n, it creates W−n+1 grams and examines n words per gram: O((W−n+1)n) word operations. With fixed n=3, that is O(W).

**Deeper explanation:** Each gram is also a newly assembled string, so character-copy cost depends on the lengths of the included words. Storage is proportional to output grams and characters.

**Reference:** [`TextProcessor.cpp`](../TextProcessor.cpp), `generateNGrams`.

**Follow-up:** What happens for n<=0 or n>W? The method returns an empty vector.

### 7. Is Quick Sort manually implemented or provided by the STL?

**Concise answer:** The project implements `partition` and recursive `quickSort` itself. It does not call `std::sort`.

**Deeper explanation:** The last element is pivot; `partition` moves strictly larger scores before it, yielding descending order. Average time is O(R log R), worst time O(R²). Equal values are not stable, and the recursion stack can become O(R).

**Reference:** [`SimilarityAnalyzer.cpp`](../SimilarityAnalyzer.cpp), `partition`, `quickSort`.

**Follow-up:** Why might the last-element pivot be poor? Already ordered or all-equal values can create very unbalanced partitions.

### 8. What does the n-gram similarity actually measure?

**Concise answer:** It measures the fraction of submitted n-gram occurrences found in the reference's unique trigram set.

**Deeper explanation:** It is directional: query duplicates are counted repeatedly, while reference duplicates are removed. It is not symmetric Jaccard over two sets and should be interpreted as query coverage.

**Reference:** [`SimilarityAnalyzer.cpp`](../SimilarityAnalyzer.cpp), `ngramSimilarity`.

**Follow-up:** How could it be made symmetric? Deduplicate both sides and divide intersection by union (or another explicitly defined symmetric score).

## Formula and score questions

### 9. Explain the three text-similarity metrics.

**Concise answer:** Jaccard compares unique-word sets; cosine compares word-frequency vectors; n-gram containment compares ordered three-word sequences.

**Deeper explanation:** Jaccard is `|A∩B|/|A∪B|`; cosine is dot product divided by vector magnitudes; the third is matches divided by submitted gram count. They capture different forms of lexical overlap, but none understands meaning.

**Reference:** [`MATHEMATICAL_EXPLANATION.md`](MATHEMATICAL_EXPLANATION.md).

**Follow-up:** Why combine them? Different measures emphasize distinct versus repeated terms and local word order; the chosen weighting is a project design choice, not a universal standard.

### 10. What are the final similarity weights?

**Concise answer:** `0.30*Jaccard + 0.50*cosine + 0.20*ngram`.

**Deeper explanation:** Weights sum to one and the components are ordinarily in [0,1], so final score remains fractional. The report multiplies it by 100 for percent display; the before/after summary currently omits that conversion.

**Reference:** `SimilarityAnalyzer::analyze`; [findings](BUGS_AND_RECOMMENDATIONS.md).

**Follow-up:** What if weights changed? Rankings and thresholds can change; weights should be justified and evaluated on labeled test cases, not tuned only for a demonstration.

### 11. How is sentence-length variance calculated?

**Concise answer:** It is the population variance: sum of squared deviations from mean divided by the number of retained sentences.

**Deeper explanation:** It differs from sample variance, which divides by S−1. Sentences are delimited by `.?!`; empty fragments are skipped; a final fragment without terminal punctuation is included.

**Reference:** [`AIDetector.cpp`](../AIDetector.cpp), `extractFeatures`.

**Follow-up:** What happens when there are no tokens? The function returns the default-zero `FeatureVector` before sentence calculations.

### 12. Why is the AI score not proof of AI authorship?

**Concise answer:** It is a deterministic heuristic over a few surface features with hand-selected weights, not a trained or validated authorship model.

**Deeper explanation:** Human and machine writing distributions overlap; style varies by topic, language, length, and writer. The project provides no calibration dataset, error rates, confidence intervals, or authorship evidence. Its number should only be described as the output of this heuristic.

**Reference:** [`AIDetector.cpp`](../AIDetector.cpp), `calculateScore` / `classify`.

**Follow-up:** What would improve it? A representative labeled dataset, held-out evaluation, calibration, bias analysis, and cautious reporting—while still avoiding claims of proof.

## Architecture, testing, and edge cases

### 13. What happens if the reference corpus is empty or missing?

**Concise answer:** A missing directory is logged; `loadDocuments` returns with an empty vector. `analyzeAll` returns an empty result vector, and the report says no reference documents were found.

**Deeper explanation:** A genuinely empty directory follows the same report path. An existing non-directory or filesystem iteration failure can throw because exceptions are not caught.

**Reference:** [`DocumentManager.cpp`](../DocumentManager.cpp), [`ReportGenerator.cpp`](../ReportGenerator.cpp).

**Follow-up:** Does low similarity then mean original text? No, it only means no measured overlap with loaded references—and with no references, no comparison occurred.

### 14. What happens if the submitted text contains no words?

**Concise answer:** Feature extraction returns all-zero features; score calculation still returns 15 because of the inverse-repetition term. Similarity metrics return zero for empty vectors, and the user still sees reports/prompts.

**Deeper explanation:** `main` has no empty-token guard. This creates a misleading nonzero heuristic score for empty or punctuation-only input.

**Reference:** `main`; `AIDetector::extractFeatures` and `calculateScore`.

**Follow-up:** What should the program do? Validate nonempty tokenization and show a clear input error or define an explicitly neutral empty score.

### 15. Does low similarity against this corpus prove originality?

**Concise answer:** No. It only measures lexical overlap with twelve local synthetic reference files.

**Deeper explanation:** The program does not search external sources, citations, paraphrased meaning, or unpublished documents. A small corpus cannot establish absence of copying.

**Reference:** [`data/README.txt`](../data/README.txt), [`DocumentManager.cpp`](../DocumentManager.cpp).

**Follow-up:** What is a defensible report label? “Similarity against the configured local corpus,” not “original” or “plagiarism-free.”

### 16. How do you know rephrasing preserves meaning?

**Concise answer:** The implementation does not verify meaning preservation.

**Deeper explanation:** It applies fixed regex substitutions such as `important`→`significant` and `due to the fact that`→`because`. These may fail in context, alter nuance or grammar, and are not backed by semantic checks.

**Reference:** [`Paraphraser.cpp`](../Paraphraser.cpp).

**Follow-up:** What validation is possible? Human review and semantic/grammar evaluations; do not claim guaranteed preservation.

### 17. Why does `DocumentManager::getDocuments()` return a reference?

**Concise answer:** To avoid copying every stored document and its indexes.

**Deeper explanation:** It returns a const alias, preserving read-only access. The manager must outlive its use, which is true in `main`.

**Reference:** [`DocumentManager.cpp`](../DocumentManager.cpp), `getDocuments`.

**Follow-up:** What could invalidate the reference? Destroying the manager or mutating/reallocating its vector.

### 18. What does `cin.ignore(max, '\n')` do after a choice?

**Concise answer:** It discards the remainder of that input line after reading a single character.

**Deeper explanation:** This prevents remaining characters/newline from being consumed unexpectedly by subsequent input. It does not fix extraction failure; `choice` should be initialized and stream state checked.

**Reference:** [`main.cpp`](../main.cpp), choice handling.

**Follow-up:** What if the user types `yes`? `choice` becomes `y`, then the rest of the line is discarded, so it is accepted.

### 19. Why can building every `.cpp` together fail?

**Concise answer:** Three files define their own `main`: production `main.cpp`, `tempCodeRunnerFile.cpp`, and `test_paraphraser.cpp`.

**Deeper explanation:** A linked executable must have exactly one entry point. Build production modules explicitly and keep the paraphraser smoke driver in a separate target.

**Reference:** Workspace inventory / [`PROJECT_OVERVIEW.md`](PROJECT_OVERVIEW.md).

**Follow-up:** Is `std::filesystem` a reason for C++17? Yes; it is standardized in C++17.

### 20. What is the difference between theoretical and measured complexity here?

**Concise answer:** Complexity describes how operation counts grow with input size; a benchmark measures time on a particular machine and data set.

**Deeper explanation:** Hash operations are average O(1) but can degrade; Quick Sort has average O(R log R) and worst O(R²). The documentation's bounds are theoretical, not runtime measurements.

**Reference:** [`FUNCTION_REFERENCE.md`](FUNCTION_REFERENCE.md).

**Follow-up:** How would you benchmark? Fix corpus/input sizes, measure repeated runs, document compiler/build flags and machine, and separate file I/O from analysis timing.
