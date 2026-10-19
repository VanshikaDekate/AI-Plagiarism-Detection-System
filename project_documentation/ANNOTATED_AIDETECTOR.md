# Annotated `AIDetector`

Source: [`AIDetector.h`](../AIDetector.h), [`AIDetector.cpp`](../AIDetector.cpp).

## Types and state

`FeatureVector` is an aggregate of seven `double` fields, all initialized to `0.0`: average sentence length, sentence-length variance, average word length, vocabulary richness, repetition ratio, punctuation rate, and function-word rate. A default instance is therefore safe to read, including for empty-token input.

`AIDetector` owns `vector<string> functionWords`; the constructor fills it with 27 fixed English words. `isFunctionWord` is private. Other than that list, detector methods are `const` and do not retain per-input state.

## Constructor and helper

- Constructor (cpp line 29) assigns the braced list of words. This fixed list is an English heuristic, not a learned language model.
- `isFunctionWord(const string& word) const` (line 67) uses `std::find` from `<algorithm>` to scan from begin to end and returns whether the iterator differs from `end()`. Complexity O(F) per word, where F is dictionary size (27 here); for a tiny fixed list this is simple but a hash set would give average O(1).

## `extractFeatures` (line 103)

1. Starts with default-zero `FeatureVector features`, constructs a `TextProcessor`, cleans the original `text`, and tokenizes it.
2. If `words.empty()`, returns the zero features immediately. Thus punctuation rate also remains zero for punctuation-only input.
3. Builds `unordered_map<string,size_t> frequency` by incrementing once per token, and `unordered_set<string> uniqueWords` by inserting tokens. Both are O(W) average, with O(W) storage.
4. `totalCharacters` sums `word.length()` after explicit conversion to `double`; `avgWordLength = totalCharacters / words.size()` with the denominator explicitly converted. The early empty-word return prevents division by zero.
5. `vocabularyRichness = uniqueWords.size()/words.size()` after casting the numerator to double; values are in (0,1].
6. `repeatedWords` adds `count-1` for each key with count greater than one. This counts extra occurrences, not the number of distinct repeated word types. The ratio divides extras by W, in [0,1).
7. `functionWordCount` checks every token through linear `isFunctionWord`; rate is function-word tokens / W.
8. Counts characters for which `ispunct(static_cast<unsigned char>(ch))` is true and divides by `text.size()` if nonempty. This is a byte-relative punctuation rate over the original text, not the cleaned text.
9. Sentence parsing reads original characters through `stringstream::get`. Every `.`, `!`, or `?` closes a sentence; it tokenizes accumulated `currentSentence` and records its nonempty word count, then clears it. A final unterminated fragment is handled after the loop. Multiple adjacent terminators may produce empty fragments, which are ignored.
10. A loop obtains the sentence-length sum with explicit conversions to `double`; mean is sum / number of retained sentences. Variance is population variance `Σ(length-mean)² / numberOfSentences`, not sample variance. The function returns the aggregate.

For N input characters and W words, cleaning/tokenization are O(N); map/set operations average O(W); function-word checks cost O(WF); sentence parsing/tokenization adds O(N). With fixed F, total is O(N) expected and O(N) auxiliary storage. Worst-case hash collisions can increase hash work.

## `calculateScore` (line 389)

For feature vector `f`, define normalized values:

```text
s = min(avgSentenceLength / 30, 1)
v = min(sentenceLengthVariance / 100, 1)
w = min(avgWordLength / 10, 1)
d = min(vocabularyRichness, 1)
r = min(repetitionRatio, 1)
p = min(punctuationRate * 10, 1)
g = min(functionWordRate * 2, 1)
```

Then:

```text
score = 100 * (0.20s + 0.10v + 0.10w + 0.20d
               + 0.15(1-r) + 0.10p + 0.15g)
```

The method first returns zero if any field is non-finite, and returns zero if all feature values are nonpositive (the extracted empty-input case). Each normalized feature is clamped to [0,1], and the final score is clamped to [0,100], including for manually constructed invalid or extreme values. The weights and thresholds remain hand-chosen and are not a calibrated probability.

## `classify` (line 470)

Returns `"Higher AI-like pattern"` at score >=70, `"Moderate AI-like pattern"` at >=40, otherwise `"Lower AI-like pattern"`. It does not validate score range. It is threshold-based text labeling, not machine learning and not evidence of authorship.
