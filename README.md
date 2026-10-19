# AI & Plagiarism Detection and Rephrasing System

A C++17 console project that extracts simple linguistic features, measures lexical overlap against a local reference corpus, and applies a small fixed set of phrase and synonym substitutions.

## What it does

- Normalizes text, tokenizes it, and generates trigrams.
- Computes a heuristic AI-likeness score from hand-selected features and weights. It is not a trained model and cannot prove who authored text.
- Compares word sets, word-frequency vectors, and trigram sets with the `.txt` files under `data/reference`.
- Ranks local-reference comparisons and reports the individual components.
- Optionally performs dictionary-based phrase/word substitutions and re-runs analysis. This is not semantic paraphrasing and does not guarantee meaning or grammar preservation.

Similarity reports describe lexical overlap with the configured local corpus only. They are not proof of plagiarism or originality; the included corpus is limited synthetic test material.

Run the application from the repository root so the relative path `data/reference` resolves correctly.

## Build

With Clang or GCC supporting C++17, compile the production sources explicitly:

```sh
clang++ -std=c++17 -Wall -Wextra -Wpedantic -Wconversion -Wshadow \
  main.cpp TextProcessor.cpp Document.cpp DocumentManager.cpp \
  AIDetector.cpp SimilarityAnalyzer.cpp ReportGenerator.cpp Paraphraser.cpp \
  -o /tmp/AIPlagiarismSystem
```

Do not compile `tempCodeRunnerFile.cpp` or `test_paraphraser.cpp` into the production executable; each contains a separate `main()`.

## Tests

Compile and run the deterministic core tests:

```sh
clang++ -std=c++17 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -I. \
  tests/test_core.cpp TextProcessor.cpp Document.cpp DocumentManager.cpp \
  AIDetector.cpp SimilarityAnalyzer.cpp ReportGenerator.cpp Paraphraser.cpp \
  -o /tmp/test_core
/tmp/test_core
```

Run end-to-end input and workflow tests against a production build:

```sh
sh tests/test_workflow.sh /tmp/AIPlagiarismSystem
```

The standalone `test_paraphraser.cpp` is also a simple assertion-style executable; compile it separately with `Paraphraser.cpp`.
