# Annotated `SimilarityAnalyzer`

Source: [`SimilarityAnalyzer.h`](../SimilarityAnalyzer.h), [`SimilarityAnalyzer.cpp`](../SimilarityAnalyzer.cpp).

## Types and public API

`SimilarityResult` contains `filename`, three component scores (`jaccard`, `cosine`, `ngram`), combined `finalScore`, and a vector of matching trigram strings. It is populated by `analyze` and returned by value. `SimilarityAnalyzer` privately implements `partition` and `quickSort`; its public functions calculate each metric, analyze one document, and analyze/rank the full corpus.

The header forward-declares `Document` to avoid including its full definition there. `SimilarityAnalyzer.cpp` includes `Document.h`, where the full data members are needed. `unordered_map` is part of the `analyzeAll` signature.

## `jaccard` (cpp line 32)

1. Builds `set1` and `set2` from word vectors using iterator-range constructors. Duplicate occurrences disappear.
2. Traverses `set1`; a hash lookup in `set2` increments `intersection` when present.
3. `unionSize = |set1| + |set2| - intersection`; if zero, returns 0.0 rather than dividing by zero. Otherwise returns intersection / union.

Expected O(W1+W2) time and O(U1+U2) storage, where W is token count and U distinct token count; worst-case hash behavior may be higher. Empty/empty similarity is explicitly defined as zero here.

## `cosine` (line 93)

Creates `freq1` and `freq2` count maps using `size_t` counts. For each unique word in `freq1`, it finds a counterpart in `freq2` and adds the product of their double-converted counts to `dotProduct`. It accumulates squared counts into `magnitude1` and `magnitude2`; these variables store squared Euclidean norms, despite their names. If either is zero, returns 0. Otherwise it computes and clamps:

```text
dotProduct / (sqrt(magnitude1) * sqrt(magnitude2))
```

This is cosine similarity over bag-of-words term-frequency vectors; it ignores word order. Expected O(W1+W2+U1+U2) and O(U1+U2) space. Counts avoid `int` overflow; the returned component is constrained to [0,1].

## `ngramSimilarity` (line 175)

Constructs unique sets from both vectors and returns `|A∩B| / |A∪B|`, clamped to [0,1]. If both sets are empty, returns zero. Duplicate trigrams are ignored on both sides, so the metric is symmetric Jaccard over trigram sets. Expected O(Nq+Nr) time and O(Uq+Ur) storage.

## `analyze` (line 225)

Copies `filename` into a new `SimilarityResult`, assigns Jaccard, cosine, and trigram components using the methods above, then calculates:

```text
finalScore = 0.30*jaccard + 0.50*cosine + 0.20*ngram
```

It constructs a reference trigram set and appends each distinct submitted trigram found there to `matchingPhrases`, preserving first submitted occurrence order. Returns the result by value. Per-document complexity is the sum of all metric work plus O(Nq+Nr) expected for the matching set/list.

## `partition` and `quickSort` (lines 328 and 385)

`partition(results, low, high)` uses `size_t` bounds, chooses the last element's `finalScore` as pivot, and tracks the next insertion position for a strictly greater score. It scans `j` from `low` to `high-1`, swaps greater values left, then swaps the pivot into place and returns the index. This avoids signed/unsigned index conversions. The `>` comparison gives descending order; ties are not stable.

`quickSort` is a hand-written Quick Sort (not `std::sort`). It recurses on the smaller side and iterates over the larger side, retaining average O(R log R), worst-case O(R²) comparisons, and O(log R) auxiliary stack space. The last-element pivot can still produce unbalanced comparisons for ordered or all-equal scores.

## `analyzeAll` (line 448)

1. Accepts submitted words, the precomputed `frequency`, submitted n-grams, and `vector<Document>`.
2. `(void)frequency;` explicitly marks the map unused. The `cosine` method rebuilds frequency maps from word vectors, so this parameter does not improve runtime.
3. For each `Document`, creates `documentNgrams` by copying its unordered set into a vector; this order is arbitrary but metrics use membership/counts rather than ordering.
4. Calls `analyze` with submitted vectors and the document's filename and indexed words, then appends each result.
5. If results are nonempty, calls `quickSort(results, 0, results.size() - 1)` using `size_t` bounds.
6. Returns descending results. Empty corpus returns an empty vector.

For R references and per-reference token/gram sizes, expected analysis cost is the sum of per-reference metrics plus Quick Sort O(R log R) average. It does not compare documents with one another.
