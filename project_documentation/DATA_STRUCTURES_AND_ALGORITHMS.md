# Data structures and algorithms actually used

## `string`

`std::string` stores original, cleaned, token, n-gram, and filename text. Strings are mutable owned sequences of `char`; appending may reallocate, hence `cleanText` calls `reserve`. Character operations are byte-oriented. Appending/copying cost is proportional to characters copied, not always constant.

## `vector`

Used for ordered tokens, generated submitted n-grams, document collections, feature sentence lengths, matching phrases, synonym alternatives, and results. A vector is a contiguous dynamic array: indexed access is O(1); append is amortized O(1), potentially O(n) during reallocation; iteration is O(n). Order and duplicates are preserved. It is appropriate for words because n-grams require neighboring positions. `vector<Document>` and `vector<SimilarityResult>` also support sequential corpus processing and sorting.

## `unordered_map`

Used for word → integer frequency in `main`, `Document`, and cosine computations, plus replacement rule maps. Hash tables provide average O(1) lookup/insert/update and O(n) traversal; worst-case operations can be O(n) under collisions. `frequency[word]++` inserts a missing key with default value zero. `analyzeAll` accepts a frequency map but does not use it, so it currently duplicates counting performed inside `cosine`.

## `unordered_set`

Used for unique document trigrams, unique vocabulary, and membership-based similarity. Insert/find are average O(1); iteration order is unspecified. Jaccard uses sets for both word lists; cosine uses counts instead. Reference trigrams are intentionally deduplicated in `Document`, while submitted trigrams retain duplicates as a vector.

## Algorithms implemented directly in project code

### Normalization and tokenization

`cleanText` performs a character scan, lowercases alphabetic bytes, keeps whitespace and `.?!`, and substitutes all other bytes with spaces. A string stream then collapses whitespace. `tokenize` extracts whitespace-delimited chunks and strips terminal `.?!`. Both are linear scans in ordinary inputs; neither is a linguistic parser or Unicode tokenizer.

### N-gram generation

`generateNGrams` uses a sliding window: for W words and width n it generates `max(0,W-n+1)` adjacent sequences. It manually joins each window with spaces. Fixed n=3 gives linear word-level work.

### Word frequency and feature aggregation

Frequency counting is performed by repeated hash-map increments. Unique vocabulary uses set insertion. Sentence splitting is a scan for `.?!`, followed by tokenizing each fragment. Feature calculations aggregate values by loops.

### Similarity metrics

- Jaccard: unique-word intersection divided by union.
- Cosine: term-frequency vector dot product divided by product of Euclidean norms.
- N-gram score: Jaccard similarity of unique submitted and reference trigram sets. Both sides are deduplicated; if both are empty the score is zero.
- Final score: 30% Jaccard + 50% cosine + 20% trigram Jaccard.

### Quick Sort

`partition` selects the last result score as pivot, moves strictly larger scores left, then places pivot between partitions. `quickSort` recurses on the smaller partition and loops through the larger. This is manually implemented Quick Sort; the project does not call `std::sort`. Average O(R log R), worst O(R²); the fixed last-item pivot can still produce quadratic comparisons, while smaller-side recursion keeps stack space O(log R). It is unstable, so equal-score order is not guaranteed.

### Phrase and synonym replacement

`Paraphraser` uses `std::regex` / `std::regex_replace` with case-insensitive whole-word boundaries. Phrase substitutions run before word substitutions. The map iteration order is unspecified. This is a fixed rule system, not contextual paraphrasing or an AI language model.

## STL-provided operations versus project algorithms

`std::find`, string streams, `std::regex_replace`, filesystem directory iteration, and `vector`/hash-container internals are library facilities. Their underlying implementations are not hand-coded here. The application itself does implement n-gram windows, feature aggregation, Jaccard/cosine calculations, phrase matching orchestration, and Quick Sort.

## Techniques not present

No binary search, trie, custom hash function/table, manually implemented partition library, trained machine-learning classifier, external search/index, database, or semantic embedding is present. In particular, AI detection is a deterministic weighted heuristic.
