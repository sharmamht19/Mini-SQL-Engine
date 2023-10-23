// #include "cursor.h"
// #include "bufferManager.h"
#include <bits/stdc++.h>
using namespace std;
/**
 * @brief The Matrix class holds all information related to a loaded matrix. It
 * also implements methods that interact with the parsers, executors, cursors
 * and the buffer manager.
 *
 */
class Matrix
{
public:
    string matrixName;
    string matrixSourceFileName;
    // dimensions of actual matrix
    int rowsDim, colsDim;
    // #pages required along a row or column
    int pages;

    Matrix(string matrixName);
    bool load();
    bool matrixBlockify();
    void setDimensions(string firstRow);
    vector<int> insertRowInPage(string rowData, int colNum);
    void doTranspose();
    void doCheckSymmetry();
    void renameMatrix(string renameTo);
    bool isPermanent();
    vector<int> makePermanent(string exportMatrixDest, string fileName, int flag);
    void writeDataToFile(vector<int> &data, ofstream &fout, int count, int flag);
    void computeOnMatrix();
    void print();
};