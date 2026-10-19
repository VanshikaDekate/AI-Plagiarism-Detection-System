#include "DocumentManager.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

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

    stringstream buffer;

    buffer << file.rdbuf();

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

    if (!fs::exists(directoryPath))
    {
        cerr << "Reference directory does not exist: "
             << directoryPath << endl;

        return;
    }


    /*
    --------------------------------------------------------
    ALGORITHM:
        Directory iteration
    --------------------------------------------------------
    */

    for (const auto& entry :
         fs::directory_iterator(directoryPath))
    {
        // Ignore directories.
        if (!entry.is_regular_file())
            continue;


        // Only process .txt files.
        if (entry.path().extension() != ".txt")
            continue;


        string filename = entry.path().filename().string();

        string fullPath = entry.path().string();


        // Read file contents.
        string content = readFile(fullPath);


        if (!content.empty())
        {
            // Create a Document object.
            Document document(filename, content);


            /*
            DATA STRUCTURE:
                vector<Document>

            Store the processed document.
            */
            documents.push_back(document);
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