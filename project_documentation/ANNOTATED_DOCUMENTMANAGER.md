# Annotated `DocumentManager`

Source: [`DocumentManager.h`](../DocumentManager.h), [`DocumentManager.cpp`](../DocumentManager.cpp).

## Interface and ownership

The class privately owns `vector<Document> documents`. The public operations load from a directory, read a file, and return a const reference to the collection. The const reference avoids a potentially large copy and prevents callers from mutating the vector through that reference.

`DocumentManager.h` includes `Document.h` because `vector<Document>` needs the complete `Document` type at member declaration. Its `string` and `vector` dependencies are included directly.

## `readFile` (cpp line 32)

1. `ifstream file(filename)` opens the named path in text mode. The stream is an automatic local and closes through RAII on every return path.
2. If `is_open()` is false, it writes an error to `cerr` and returns an empty string. Since empty files also yield empty content, callers cannot distinguish a failed read from a valid empty file by return value alone.
3. Otherwise an 8192-byte buffer is repeatedly filled with `file.read` and appended to an `ostringstream`. A final short read at EOF is retained; `badbit` or a non-EOF failure is reported. The resulting string owns the complete file content. For N file bytes, time and storage are O(N).

## `loadDocuments` (line 77)

1. `documents.clear()` removes the previous collection before attempting a reload.
2. `fs::exists` and `fs::is_directory` use `error_code` overloads. Missing, non-directory, and status errors are reported, then the function returns with an empty collection.
3. `namespace fs = std::filesystem` aliases the C++17 filesystem namespace.
4. `directory_iterator` visits immediate directory entries (not recursive traversal). Non-regular files are skipped; extension comparisons accept only exactly `.txt`. Iterator construction, entry status, and increments are checked with `error_code`; traversal problems are reported.
5. `filename` is the basename shown in reports; `fullPath` is the path used to read. `readFile(fullPath)` creates `content`. Empty files are explicitly reported and skipped; failed opens/reads are logged to `cerr`.
6. `Document document(filename, content)` indexes the contents; `documents.push_back(std::move(document))` moves the indexed object into the vector. Finally the function prints loaded count.

Expected total time is proportional to total file bytes/tokens plus directory entries, under average hash behavior. It holds every document's raw text and indexes in memory. Iteration order is unspecified, so equal-scoring tie order may vary.

## `getDocuments` (line 149)

Returns `const vector<Document>&` to the manager-owned vector. The reference is valid only while its owner remains alive and the vector is not invalidated by mutation. In `main`, the manager remains alive throughout use.

## Error and path assumptions

The path in `main` remains relative to current working directory. Filesystem status and iteration errors are reported instead of escaping as filesystem exceptions. `readFile` still returns an empty string for both an empty file and a failed read; however, failures are logged and `loadDocuments` separately reports that empty content was skipped.
