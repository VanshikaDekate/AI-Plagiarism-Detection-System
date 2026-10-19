#include "DocumentManager.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>

using namespace std;

// C++17 filesystem namespace
namespace fs = std::filesystem;


/*
============================================================
FUNCTION: readFile()
============================================================

ALGORITHM:
    Sequential file reading.

DATA STRUCTURE:
    string

TIME COMPLEXITY:
    O(N)
    N = file size

============================================================
*/

string DocumentManager::readFile(
    const string& filename) const
{
    ifstream file(filename);

    if (!file.is_open())
    {
        cerr << "Error: Could not open file: "
             << filename << endl;

        return "";
    }

    ostringstream buffer;

    constexpr streamsize chunkSize = 8192;
    char chunk[8192];
    while (file.read(chunk, chunkSize) || file.gcount() > 0)
    {
        buffer.write(chunk, file.gcount());
    }

    if (file.bad() || (file.fail() && !file.eof()))
    {
        cerr << "Error: Failed while reading file: "
             << filename << endl;
        return "";
    }

    return buffer.str();
}


/*
============================================================
FUNCTION: loadDocuments()
============================================================

PURPOSE:
    Load every .txt file inside the reference directory.

ALGORITHM:
    Directory traversal.

DATA STRUCTURE:
    vector<Document>

STEPS:
    1. Traverse directory.
    2. Find .txt files.
    3. Read each file.
    4. Create Document object.
    5. Store Document in vector.

============================================================
*/

void DocumentManager::loadDocuments(
    const string& directoryPath)
{
    documents.clear();

    error_code error;
    const fs::path directory(directoryPath);

    if (!fs::exists(directory, error))
    {
        if (error)
        {
            cerr << "Error checking reference directory "
                 << directoryPath << ": "
                 << error.message() << endl;
        }
        else
        {
            cerr << "Reference directory does not exist: "
                 << directoryPath << endl;
        }

        return;
    }

    if (!fs::is_directory(directory, error) || error)
    {
        cerr << "Reference path is not a readable directory: "
             << directoryPath;
        if (error)
        {
            cerr << " (" << error.message() << ")";
        }
        cerr << endl;

        return;
    }


    /*
    --------------------------------------------------------
    ALGORITHM:
        Directory iteration
    --------------------------------------------------------
    */

    fs::directory_iterator entry(directory, error);
    const fs::directory_iterator end;

    if (error)
    {
        cerr << "Error opening reference directory "
             << directoryPath << ": "
             << error.message() << endl;
        return;
    }

    while (entry != end)
    {
        const fs::directory_entry& current = *entry;
        error.clear();

        // Ignore directories.
        const bool regularFile = current.is_regular_file(error);
        if (error)
        {
            cerr << "Error inspecting reference entry "
                 << current.path().string() << ": "
                 << error.message() << endl;
        }
        else if (regularFile &&
                 current.path().extension() == ".txt")
        {
            const string filename =
                current.path().filename().string();
            const string fullPath = current.path().string();

            // Read file contents.
            string content = readFile(fullPath);

            if (!content.empty())
            {
                // Create a Document object.
                Document document(filename, content);

                documents.push_back(std::move(document));
            }
            else
            {
                cout << "Ignoring empty reference file: "
                     << filename << endl;
            }
        }


        error.clear();
        entry.increment(error);
        if (error)
        {
            cerr << "Error traversing reference directory "
                 << directoryPath << ": "
                 << error.message() << endl;
            break;
        }
    }


    cout << documents.size()
         << " reference documents loaded."
         << endl;
}


/*
============================================================
FUNCTION: getDocuments()
============================================================
*/

const vector<Document>&
DocumentManager::getDocuments() const
{
    return documents;
}