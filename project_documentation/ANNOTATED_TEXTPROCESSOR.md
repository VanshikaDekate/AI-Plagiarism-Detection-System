# Annotated `TextProcessor`

Source: [`TextProcessor.h`](../TextProcessor.h), [`TextProcessor.cpp`](../TextProcessor.cpp).

## Header

- `TEXTPROCESSOR_H` guards the header against duplicate inclusion in one translation unit.
- `<string>` and `<vector>` declare the public types. `using namespace std;` makes their names unqualified in this header; putting a using-directive in a header also affects every includer.
- `TextProcessor` has no data members and no explicit constructor, so the compiler supplies a default constructor.
- `cleanText(const string& text)` takes a read-only reference to avoid copying the input and returns a new owned normalized string.
- `tokenize(const string& text)` returns an ordered dynamic array of word strings.
- `generateNGrams(const vector<string>& words, int n)` reads the input sequence by const reference and returns all adjacent windows of `n` words. `int` permits validation of zero and negative sizes.

## `cleanText` (`TextProcessor.cpp`, line 33)

1. `string cleaned;` creates the result; `reserve(text.size())` requests enough capacity for a worst-case one-output-character-per-input-character pass. It is an optimization, not a logical requirement.
2. The range-based loop reads each `char ch`. Calls to `isalpha`, `tolower`, and `isspace` first cast to `unsigned char`. This avoids undefined behavior for negative signed-char values, an important requirement of the C character-classification functions.
3. Alphabetic input is lowercased and appended. Whitespace is appended as one ordinary space. `.`, `!`, and `?` are preserved as sentence punctuation. Every other character becomes a space: commas, apostrophes, digits, hyphens, and other punctuation are not retained as tokens.
4. `stringstream ss(cleaned)` plus repeated `ss >> word` collapses runs of whitespace and removes leading/trailing whitespace. `normalized` joins extracted chunks with exactly one ASCII space. It does not collapse or remove sentence-ending marks.
5. Returns `normalized` by value; modern C++ can elide/move the result. Runtime is linear in character count, with stream/string allocation overhead; auxiliary storage is O(N).

## `tokenize` (line 119)

1. Creates an empty `vector<string> words`, a stream over `text`, and one reusable `word`.
2. Each formatted extraction reads a whitespace-delimited token. A nested loop removes any number of terminal `.`, `!`, and `?` characters using `back()` and `pop_back()`.
3. The emptiness guard prevents storing a token consisting only of stripped terminal punctuation. Nonempty tokens are appended in source order.
4. Returns all tokens. For whitespace-tokenized input this is O(N) total character processing and O(W) token/container storage, where N is characters and W is token count. It does not independently lowercase or remove internal punctuation; normal use passes `cleanText` output.

## `generateNGrams` (line 178)

1. Creates `ngrams`. `if (n <= 0 || words.size() < static_cast<size_t>(n))` returns it when n is invalid or the window cannot fit. `||` short-circuiting ensures a negative `n` is not converted to an unsigned size before the first check.
2. The outer `size_t i` iterates starting positions `0` through `words.size()-n`. The inner `int j` concatenates exactly n words, adding one space between them, and appends each completed gram.
3. For W words and valid n, output count is W-n+1. Construction takes O((W-n+1)·n) word appends, plus character-copy cost proportional to the total gram-string lengths; if n is fixed (the program uses 3), this is linear in W. Output storage is O((W-n+1)·n) words/characters.

## Important behavior and limitations

- The tokenizer is whitespace-based, not a general linguistic tokenizer.
- Cleaning uses locale-sensitive `<cctype>` functions on individual bytes; it is not Unicode-aware.
- Sentence marks are preserved during cleaning but stripped only from token endings.
- Example: `"AI, systems!"` cleans to `"ai  systems!"`, normalizes to `"ai systems!"`, and tokenizes to `["ai", "systems"]`.
- A zero or negative n returns an empty vector rather than throwing.
