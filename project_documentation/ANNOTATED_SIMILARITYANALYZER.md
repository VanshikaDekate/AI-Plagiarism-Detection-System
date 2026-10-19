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

Creates `freq1` and `freq2` count maps from the token vectors. For each unique word in `freq1`, it looks for a counterpart in `freq2` and adds `count1*count2` to `dotProduct`. It accumulates each map's squared counts into `magnitude1` and `magnitude2`; these variables store squared Euclidean norms, despite their names. If either is zero, returns 0. Otherwise returns:

```text
dotProduct / (sqrt(magnitude1) * sqrt(magnitude2))
```

This is cosine similarity over bag-of-words term-frequency vectors; it ignores word order. Expected O(W1+W2+U1+U2) and O(U1+U2) space. Integer counts are multiplied before promotion to `double`, so extremely large counts could overflow `int`.

## `ngramSimilarity` (line 175)

Returns 0 when submitted n-grams are empty. Otherwise it creates a set from reference n-grams, counts every submitted gram found in that set, and divides by the submitted vector's length. This is **query-side containment/recall-like overlap**, not symmetric set Jaccard: reference grams are membership-tested, while duplicate query grams each count and affect the denominator. In `Document`, reference grams are already unique. Expected O(Nq+Nr) time and O(Ur) extra set storage.

## `analyze` (line 225)

Copies `filename` into a new `SimilarityResult`, assigns Jaccard, cosine, and trigram components using the methods above, then calculates:

```text
finalScore = 0.30*jaccard + 0.50*cosine + 0.20*ngram
```

It constructs a reference trigram set again and appends each submitted trigram found there to `matchingPhrases`; repeated submitted trigrams can therefore repeat in this vector. The report limits display to ten entries, but does not deduplicate them. Returns the result by value. Per-document complexity is the sum of all metric work plus O(Nq+Nr) to prepare the matching set/list.

## `partition` and `quickSort` (lines 328 and 385)

`partition(results, low, high)` chooses the last element's `finalScore` as pivot and sets `i = low-1`. It scans `j` from `low` to `high-1`; scores strictly greater than pivot are swapped to the left. It then swaps the pivot into `i+1` and returns that index. The `>` comparison gives descending order; ties are not stable.

`quickSort` recurses only if `low < high`, partitions, then sorts `[low,pivotIndex-1]` and `[pivotIndex+1,high]`. It is a hand-written Quick Sort (not `std::sort`). Average time O(R log R), worst-case O(R²), with recursion stack average O(log R), worst O(R), for R results. Choosing the last item as pivot and using strict comparison can produce highly unbalanced partitions for already ordered data or all-equal scores.

## `analyzeAll` (line 448)

1. Accepts submitted words, the precomputed `frequency`, submitted n-grams, and `vector<Document>`.
2. `(void)frequency;` explicitly marks the map unused. The `cosine` method rebuilds frequency maps from word vectors, so this parameter does not improve runtime.
3. For each `Document`, creates `documentNgrams` by copying its unordered set into a vector; this order is arbitrary but metrics use membership/counts rather than ordering.
4. Calls `analyze` with submitted vectors and the document's filename and indexed words, then appends each result.
5. If results are nonempty, calls `quickSort(results, 0, static_cast<int>(results.size()) - 1)`. A corpus too large for `int` indexing would overflow/narrow; practical corpus sizes are small.
6. Returns descending results. Empty corpus returns an empty vector.

For R references and per-reference token/gram sizes, expected analysis cost is the sum of per-reference metrics plus Quick Sort O(R log R) average. It does not compare documents with one another.
