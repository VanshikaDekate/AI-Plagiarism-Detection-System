#ifndef DOCUMENTMANAGER_H
#define DOCUMENTMANAGER_H

#include "Document.h"

#include <string>
#include <vector>

using namespace std;


/*
============================================================
CLASS: DocumentManager
============================================================

RESPONSIBILITY:
    Load and store all reference documents.

DATA STRUCTURE:
    vector<Document>

WHY VECTOR?
    We need to maintain a collection of reference documents
    that can later be iterated through for comparison.

ALGORITHM:
    Directory traversal + file reading.

============================================================
*/

class DocumentManager
{
private:

    // -------------------------------------------------------
    // DATA STRUCTURE:
    // vector<Document>
    //
    // Stores every reference document.
    // -------------------------------------------------------
    vector<Document> documents;


public:

    /*
    Load all .txt files from a directory.
    */
    void loadDocuments(const string& directoryPath);


    /*
    Read one file completely.
    */
    string readFile(const string& filename) const;


    /*
    Return loaded documents.
    */
    const vector<Document>& getDocuments() const;
};

#endif