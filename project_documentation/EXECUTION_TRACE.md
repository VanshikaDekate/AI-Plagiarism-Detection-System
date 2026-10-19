# Execution trace

## Startup through original report

1. The operating system enters `main()` in [`main.cpp`](../main.cpp); the console banner is printed.
2. The `getline` loop appends each line to `originalText`, preserving internal newlines. An exact `"END"` line stops collection. EOF also stops it.
3. `TextProcessor::cleanText` converts to lowercase, replaces non-alpha/non-whitespace/non-sentence-punctuation bytes with spaces, then collapses whitespace. `tokenize` extracts words. If none remain, `main` reports invalid input and exits nonzero. Otherwise `generateNGrams(words, 3)` emits each consecutive triple.
4. `main` builds `frequency` by hash-map increment and prints word and trigram vector sizes.
5. `AIDetector::extractFeatures(originalText)` independently cleans/tokenizes and derives counts, vocabulary, punctuation, sentence lengths, and variance. `calculateScore` applies normalizers and the weighted sum; `classify` applies 40/70 thresholds.
6. `DocumentManager::loadDocuments("data/reference")` clears previous docs and visits the directory's immediate entries. Each regular `.txt` file that reads as nonempty is constructed as a `Document`. Each document builds its own ordered words, count map, and unique trigram set.
7. `SimilarityAnalyzer::analyzeAll` iterates each document. For each, it rebuilds word frequencies inside `cosine`, uses unique word sets for Jaccard, uses unique submitted/reference trigram sets for symmetric trigram Jaccard, calculates a final score clamped to [0,1], and records distinct matching trigrams. It sorts all per-document results descending with custom Quick Sort.
8. `ReportGenerator::showCombinedReport` prints features and the AI score, then rankings, top-document metric breakdown, category thresholds, matching phrases, and a footer. It does not analyze.
9. `askYesNo` reads and trims a full input line, retries invalid responses, accepts `y/Y/n/N`, and safely exits the optional flow on EOF. On `y/Y`, `Paraphraser` runs fixed phrase substitutions, fixed word substitutions, and sentence-start capitalization. It prints the result and asks whether to analyze it.
10. On a second `y/Y`, the same preprocessing and detector/analyzer methods run again against the already-loaded documents; original data remains available. The program prints both reports and score differences, then returns 0.

## Verified worked run

Compiled with C++17 from the production sources (excluding the two extra source files with duplicate `main`) and ran input:

```text
A cat sees a cat.
END
n
```

Observed trace:

| Stage | Value |
|---|---|
| `originalText` | `"A cat sees a cat."` |
| cleaned text | `"a cat sees a cat."` |
| `words` | `["a", "cat", "sees", "a", "cat"]` |
| `frequency` | `a:2, cat:2, sees:1` |
| `ngrams` | `["a cat sees", "cat sees a", "sees a cat"]` |
| `FeatureVector` | sentence mean 5; variance 0; average word length 2.40; richness 0.60; repetition 0.40; punctuation rate 1/17≈0.05882; function rate 0.40 |
| AI score / class | 44.62% / Moderate AI-like pattern |
| Corpus | 12 reference documents loaded |
| Top result displayed | `data_science.txt`, Jaccard 1.52%, cosine 5.80%, trigram 0.00%, final 3.36%, low textual similarity |

The report's percentages are rounded to two decimals. Corpus iteration order can differ; scores and tie ordering should not be inferred as stable beyond the displayed run. The tiny sample's nonzero overlap is caused by common tokens, illustrating why lexical overlap is not by itself a plagiarism judgment.

## Termination and exceptional paths

Normal completion returns 0. Invalid text returns a failure status. Missing or invalid reference paths and iteration/read failures are reported; results may be empty or partial, and the report prints `"No reference documents found."` EOF at a yes/no prompt exits safely. The local corpus path remains relative to the current working directory.
