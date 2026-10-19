# Annotated `Document`

Source: [`Document.h`](../Document.h), [`Document.cpp`](../Document.cpp).

## Stored representation

`Document` is a public struct-like class whose data members are all public:

- `filename` and `content` own the basename and full raw file text.
- `words` preserves cleaned token order.
- `frequency` maps each distinct token to its count.
- `ngrams` is an `unordered_set<string>` of unique trigrams.

The vector preserves sequence and duplicates, the map preserves counts, and the set answers membership questions while deduplicating phrases. Hash operations are average O(1), worst-case O(number of keys).

## Constructors

- `Document()` (cpp line 13) has an empty body. All members default-construct to empty values. It does not call `buildIndex`, so a default document is empty.
- `Document(const string& filename, const string& content)` copies each const-reference parameter into the correspondingly named member via `this->`; the `this->` qualification disambiguates member from parameter. It then calls `buildIndex`, establishing derived data from content. Construction is O(N) average for fixed trigram length plus hash-table operations and memory for the indexed structures.

## `buildIndex` (line 69)

1. Constructs a stateless `TextProcessor`.
2. `cleanText(content)` produces a local `cleaned` copy; `tokenize(cleaned)` assigns ordered tokens to the member `words`.
3. `frequency.clear()` allows rebuilding an already-existing document without retaining stale counts. Each `frequency[word]++` increments a word count; average O(W), worst-case hash behavior can be worse.
4. `generateNGrams(words, 3)` creates a temporary ordered vector `generatedNGrams`.
5. `ngrams.clear()` prevents stale phrases on rebuild. Inserting each gram into the set stores only distinct sequences. Expected time is O(W) for fixed trigram length; set storage is O(Ug), where Ug is unique trigram count.

Calling `buildIndex` again after editing public `content` recomputes the indexes. Direct callers can also mutate public `words`, `frequency`, or `ngrams`, allowing an inconsistent state; encapsulating members would prevent this, but is not current behavior.
