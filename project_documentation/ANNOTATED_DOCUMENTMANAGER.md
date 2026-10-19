# Annotated `DocumentManager`

Source: [`DocumentManager.h`](../DocumentManager.h), [`DocumentManager.cpp`](../DocumentManager.cpp).

## Interface and ownership

The class privately owns `vector<Document> documents`. The public operations load from a directory, read a file, and return a const reference to the collection. The const reference avoids a potentially large copy and prevents callers from mutating the vector through that reference.

`DocumentManager.h` includes `Document.h` because `vector<Document>` needs the complete `Document` type at member declaration. Its `string` and `vector` dependencies are included directly.

## `readFile` (cpp line 32)

1. `ifstream file(filename)` opens the named path in text mode.
2. If `is_open()` is false, it writes an error to `cerr` and returns an empty string. Since empty files also yield empty content, callers cannot distinguish a failed read from a valid empty file by return value alone.
3. Otherwise a `stringstream buffer` receives `file.rdbuf()`, copying the stream's remaining contents into memory; `buffer.str()` returns the whole file. For N file bytes, time and storage are O(N).

## `loadDocuments` (line 77)

1. `documents.clear()` removes the previous collection before attempting a reload.
2. `fs::exists(directoryPath)` checks path existence. If false, an error is written and the function returns, leaving an empty vector.
3. `namespace fs = std::filesystem` aliases the C++17 filesystem namespace.
4. `directory_iterator` visits immediate directory entries (not recursive traversal). Non-regular files are skipped with `continue`; extension comparisons accept only exactly `.txt`.
5. `filename` is the basename shown in reports; `fullPath` is the path used to read. `readFile(fullPath)` creates `content`. Only nonempty strings are added, so empty files are silently omitted after a successful read.
6. `Document document(filename, content)` indexes the contents; `documents.push_back(document)` copies that local document into the vector. A move could avoid a copy, but the current source copies. Finally the function prints loaded count.

Expected total time is proportional to total file bytes/tokens plus directory entries, under average hash behavior. It holds every document's raw text and indexes in memory. Iteration order is unspecified, so equal-scoring tie order may vary.

## `getDocuments` (line 149)

Returns `const vector<Document>&` to the manager-owned vector. The reference is valid only while its owner remains alive and the vector is not invalidated by mutation. In `main`, the manager remains alive throughout use.

## Error and path assumptions

The path in `main` is relative to current working directory. Existence is checked, but the code does not verify that the existing path is a directory or catch `filesystem_error`; a regular file path, permissions failure, or directory iteration error may throw. Read failures log to `cerr` but are then treated similarly to empty documents.
