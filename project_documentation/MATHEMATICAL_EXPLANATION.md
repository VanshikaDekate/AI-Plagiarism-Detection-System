# Mathematical explanation

All calculations below are the formulas in the inspected implementation; they are heuristic text statistics, not calibrated probabilities.

## Linguistic features

Let W be token count, U distinct token count, `len(w_i)` token character length, and `c(w)` frequency.

| Feature | Implemented formula | Meaning / assumptions |
|---|---|---|
| Average word length | `Σ len(w_i) / W` | Mean token character count. Undefined for W=0 mathematically; implementation returns early with all features zero. |
| Vocabulary richness | `U / W` | Type-token ratio; depends on text length and does not capture semantic diversity. |
| Repetition ratio | `Σ_{w:c(w)>1}(c(w)-1) / W` | Fraction of occurrences beyond the first occurrence of each word. |
| Function-word rate | number of tokens in fixed 27-word list / W | English-specific list; no linguistic context. |
| Punctuation rate | punctuation bytes in original input / `text.size()` | Denominator is character bytes, not tokens. Returns before this calculation if W=0. |
| Sentence mean | `Σ L_i / S` | L_i is token count in each nonempty punctuation-delimited sentence; S is retained sentence count. |
| Sentence variance | `Σ(L_i-mean)^2 / S` | Population variance (divisor S), not unbiased sample variance. |

### Example

For `"A cat sees a cat."`, preprocessing yields `["a","cat","sees","a","cat"]`. W=5; unique tokens are `{a,cat,sees}`, so richness=3/5=0.60. Counts are a:2, cat:2, sees:1, so extra repetitions=1+1=2 and repetition ratio=2/5=0.40. Word lengths sum to 12, so mean=12/5=2.40. It is one 5-word sentence, so mean sentence length=5 and variance=0. The one punctuation mark over 17 bytes gives approximately 0.05882. Two of five tokens are listed function words (`a` twice), so function rate=0.40.

## AI-likeness heuristic

The code upper-caps these normalized fields:

```text
s = min(avgSentenceLength / 30, 1)
v = min(sentenceLengthVariance / 100, 1)
w = min(avgWordLength / 10, 1)
d = min(vocabularyRichness, 1)
r = min(repetitionRatio, 1)
p = min(punctuationRate * 10, 1)
g = min(functionWordRate * 2, 1)
```

Then the raw weighted result is:

```text
0.20s + 0.10v + 0.10w + 0.20d
+ 0.15(1-r) + 0.10p + 0.15g
```

The program multiplies by 100. Weights sum to 1, and each extracted normalized feature is ordinarily in [0,1]. For the example, s=1/6, v=0, w=.24, d=.60, r=.40, p≈.588235, g=.80:

```text
100 * (.20*.166667 + .10*0 + .10*.24 + .20*.60
       + .15*.60 + .10*.588235 + .15*.80)
≈ 44.6157
```

The displayed classification is `"Moderate AI-like pattern"` (40–<70). Empty `FeatureVector` gives score 15 because the term `0.15*(1-r)` contributes .15 when r=0. `min` has no lower clamp; malformed manually constructed negative feature values can produce scores outside the claimed range. The thresholds and weights are course-project choices with no calibration dataset shown.

## Jaccard similarity

For unique token sets A and B:

```text
J(A,B) = |A ∩ B| / |A ∪ B|
```

Example A={a,cat,sees}, B={a,cat,sits}; intersection=2, union=4, J=0.5. Empty union returns 0. The implementation removes duplicates before counting. Expected construction and intersection scan cost is O(W1+W2), with hash lookup.

## Cosine similarity

For term-frequency vectors x and y:

```text
cos(x,y) = (Σ_i x_i y_i) / (sqrt(Σ_i x_i²) * sqrt(Σ_i y_i²))
```

With counts x=(a:2, cat:2, sees:1) and y=(a:2, cat:2, sits:1), dot product=2·2+2·2=8, each squared norm=4+4+1=9, so cosine=8/9≈0.88889. Both zero vectors return 0. It ignores word order and uses raw counts, not TF-IDF.

## N-gram containment

The implementation computes:

```text
G = matching submitted n-gram occurrences / submitted n-gram occurrences
```

The denominator is the submitted vector length; the reference is converted to a set. Thus duplicates in the submitted vector count repeatedly and the score is asymmetric. For submitted `["a cat sees","cat sees a"]` and reference set `{"a cat sees","sees a cat"}`, score=1/2=0.5. It is not standard symmetric n-gram Jaccard despite being named similarity.

## Final similarity score and ranking

```text
Final = 0.30*J + 0.50*cos + 0.20*G
```

Using J=.5, cosine=8/9, G=.5 yields `.15 + .444444 + .10 = .694444` (69.44% when report multiplies the fractional score by 100). Weights sum to 1. `analyze` stores a fraction; reporting scales it by 100. The before/after summary in `main` does not scale it, causing mislabeled fractions.

The final score has constant-time arithmetic after the metrics. Ranking does not change the score: Quick Sort orders descending by `finalScore`, average O(R log R), worst O(R²).
