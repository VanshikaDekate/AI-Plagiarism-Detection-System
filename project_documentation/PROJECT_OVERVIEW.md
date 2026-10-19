# Project overview and inventory

## Scope and source of truth

This guide describes the checked-out workspace as inspected on 2026-10-19, including existing uncommitted edits. It does not change or normalize source code. The active entry point is [`main.cpp`](../main.cpp); `main.cpp` currently calls the paraphraser even though its closing comments still describe a future Part 2. Recommendations and defects are separated in [BUGS_AND_RECOMMENDATIONS.md](BUGS_AND_RECOMMENDATIONS.md).

The submitted reference texts are described in `data/README.txt` as synthetic test material. Results measure overlap only against those local files; they do not establish plagiarism or originality against the web, books, or other corpora.

## File inventory

| File | Responsibility and relationships |
|---|---|
| [`main.cpp`](../main.cpp) | Console entry point. Reads text, invokes preprocessing, feature extraction, document loading, similarity ranking, reports, and optionally paraphrasing and a second analysis. |
| [`TextProcessor.h`](../TextProcessor.h), [`TextProcessor.cpp`](../TextProcessor.cpp) | Character normalization, whitespace tokenization, and ordered sliding-window n-gram generation. Used by `main`, `Document`, and `AIDetector`. |
| [`Document.h`](../Document.h), [`Document.cpp`](../Document.cpp) | Represents one reference file and builds its cleaned words, word-frequency map, and unique trigram set. |
| [`DocumentManager.h`](../DocumentManager.h), [`DocumentManager.cpp`](../DocumentManager.cpp) | Traverses a directory, reads `.txt` files, constructs `Document` objects, and exposes the collection. |
| [`AIDetector.h`](../AIDetector.h), [`AIDetector.cpp`](../AIDetector.cpp) | Extracts seven hand-designed text features and combines normalized values into a heuristic 0–100 score; threshold labels are not a trained classifier. |
| [`SimilarityAnalyzer.h`](../SimilarityAnalyzer.h), [`SimilarityAnalyzer.cpp`](../SimilarityAnalyzer.cpp) | Calculates word-set Jaccard, count-vector cosine, symmetric trigram-set Jaccard, a weighted score, matching trigrams, and ranks references using manually implemented Quick Sort. |
| [`ReportGenerator.h`](../ReportGenerator.h), [`ReportGenerator.cpp`](../ReportGenerator.cpp) | Prints feature and similarity results. It does not calculate them. |
| [`Paraphraser.h`](../Paraphraser.h), [`Paraphraser.cpp`](../Paraphraser.cpp) | Performs fixed phrase and selected single-word substitutions using case-insensitive regular expressions, then capitalizes sentence starts. Called optionally from `main`. |
| [`test_paraphraser.cpp`](../test_paraphraser.cpp) | Standalone assertion-style driver with five paraphraser cases; it has its own `main`, so it must not be linked into the production executable. |
| [`tests/test_core.cpp`](../tests/test_core.cpp) | Assertion-based deterministic tests for text processing, document indexing/loading, AI feature scores, similarity formulas/ranking, paraphrasing, and report formatting. |
| [`tests/test_workflow.sh`](../tests/test_workflow.sh) | End-to-end input/branch assertions for the interactive executable. |
| [`tempCodeRunnerFile.cpp`](../tempCodeRunnerFile.cpp) | Temporary duplicate program/older `main` without the current rephrasing flow. It also defines `main`, so exclude it from a production link. |
| `AIPlagiarismSystem` | Existing compiled executable, not source and not evidence that it matches the current modified sources. |
| `test_paraphraser` | Existing compiled test executable, not source. |
| [`README.md`](../README.md) | Brief project summary. |
| [`.gitignore`](../.gitignore) | Ignores the named production executable, common object/output files, macOS metadata, VS Code settings, temporary files, and logs. |
| [`LICENSE`](../LICENSE) | MIT License. |
| [`data/README.txt`](../data/README.txt) | Explains intended categories of the local synthetic corpus. |
| `data/reference/*.txt` | Twelve local reference texts: `artificial_intelligence`, `data_science`, `education`, `environment`, `healthcare`, `healthcare_copy`, `healthcare_reworded`, `network_security`, `renewable_energy`, `software_engineering`, `technology`, and `unrelated_history`. |

No CMake, Make, Meson, or other build configuration is used. The README documents direct Clang commands. `std::filesystem` requires C++17. The production build compiles production `.cpp` files only, excluding temporary/test sources with their own `main`.

## Architecture and data flow

```text
console input
  ├─ TextProcessor: cleaned string → ordered words → ordered trigrams
  ├─ main: unordered_map word frequencies
  ├─ AIDetector: original text → FeatureVector → heuristic score/class
  ├─ DocumentManager: data/reference/*.txt → vector<Document>
  │    └─ Document: cleaned words, word frequencies, unique trigram set
  ├─ SimilarityAnalyzer: compare each reference → scores → descending order
  ├─ ReportGenerator: print original analysis
  └─ optional Paraphraser → optional repeated analysis and before/after values
```

The system is conventional procedural orchestration in `main` with classes grouping responsibilities. `Document` stores indexed data, while `DocumentManager` owns the collection. `SimilarityAnalyzer::analyzeAll` accepts a frequency map but intentionally ignores it and rebuilds frequency tables in `cosine`.

## Evidence labels used throughout

- **Implementation fact** means directly visible in this workspace.
- **Complexity** is theoretical, based on input lengths, hash-table average/worst-case behavior, and algorithm structure; it is not a benchmark.
- **Recommendation** is not behavior the current program already provides.
- **Potential risk** means a failure depends on environment or input conditions not necessarily exercised by the sample corpus.
