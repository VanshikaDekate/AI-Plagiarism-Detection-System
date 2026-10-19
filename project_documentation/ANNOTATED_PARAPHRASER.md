# Annotated `Paraphraser`

Source: [`Paraphraser.h`](../Paraphraser.h), [`Paraphraser.cpp`](../Paraphraser.cpp).

## Stored maps and constructor

`phraseMap` is an `unordered_map<string,string>` for five fixed multiword substitutions: `"due to the fact that" → "because"`, `"in order to" → "to"`, `"a large number of" → "many"`, `"as a result of" → "because of"`, `"at the present time" → "currently"`.

`synonyms` maps nine source words to vectors of candidate replacement strings. Each currently has one entry (`significant`, `approach`, `outcome`, `substantial`, `limited`, `challenging`, `precise`, `straightforward`, `fast` respectively). `replaceWords` selects `.front()`, so multiple candidates would not currently be randomized or selected contextually.

The constructor (cpp line 12) populates these maps. They are rule data, not learned vocabulary or a language-generation model.

## `replacePhrases` (line 34)

Copies input `text` to `result`. For each hash-map entry, builds regex pattern `\\b` + phrase + `\\b`, compiles it with `regex_constants::icase`, and calls `regex_replace` across the current result. Word boundaries prevent replacing a phrase embedded directly inside a longer word; case-insensitive matching accepts capitalization variations. The replacement string itself is lowercase. Because iteration order of an unordered map is unspecified, substitution order is not promised, though the current phrase keys do not materially overlap.

If text length is N and there are P rules, repeated regex construction/scanning can cost substantially more than linear time; regular-expression worst-case behavior can be superlinear. Result storage is O(N) plus regex internals.

## `replaceWords` (line 53)

Uses the same copy, case-insensitive whole-word regex technique for each synonym key, replacing matches with `entry.second.front()`. Replacement may lose original capitalization and does not inspect grammatical context. Constructor guarantees each current vector is nonempty; if future data inserted an empty vector, `.front()` would be undefined behavior.

## `rephrase` (line 72)

1. Empty input returns unchanged.
2. Applies whole-phrase replacements before single-word substitutions.
3. Scans mutable `result` by `char&`. It converts to `unsigned char` before `isalpha`/`toupper`, avoiding invalid negative-char input. `capitalizeNext` starts true; the first alphabetic character and the first alphabetic character after each `.`, `!`, or `?` is uppercased. The flag is cleared when a letter is found; punctuation resets it.
4. Returns the modified string.

This is deterministic with current nonoverlapping rules except for iteration order, and it does not prove meaning preservation. It is also byte-oriented, not Unicode-aware, and may turn sentence-initial replacement words uppercase while leaving other replacement words lowercase.

## `test_paraphraser.cpp`

The separate driver constructs one `Paraphraser`, stores six test strings in `vector<string> tests`, loops with `size_t i`, and prints each input and transformed output. The sample cases exercise phrase replacement, sentence capitalization, word-boundary behavior (`methodology`), empty input, and technical punctuation/digits. It is a manual smoke driver, not a testing framework, and has its own `main`.
