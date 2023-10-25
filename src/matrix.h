#include <bits/stdc++.h>
using namespace std;

/**
 * ========================
 * MATRIX CLASS METHODS
 * ========================
 *
 * @brief Matrix:
 * Initializes a matrix object, typically broken into smaller blocks or pages due to its large size. It provides the foundation to access and operate on matrix data.
 * the Matrix class holds all information related to a loaded matrix. It
 * also implements methods that interact with the parsers, executors, cursors
 * and the buffer manager.
 *
 * ------------------------
 *
 * @brief readPage:
 * Allows reading a specific page from the matrix based on the page index. This method fetches the page from the `matrixSourceFileName` and stores it in a memory buffer for operations.
 *
 * ------------------------
 *
 * @brief getPage:
 * Retrieves a memory buffer containing the requested matrix page. This is useful for direct matrix operations without sequentially accessing individual rows.
 *
 * ------------------------
 *
 * @brief setMatrixSourceFileName:
 * Allows the user to specify or change the source file from which the matrix is to be read. It adjusts internal pointers and controls to refer to this new source.
 *
 * ------------------------
 *
 * @brief getDimensions:
 * Returns the dimensions of the matrix in the form of rows and columns. This aids in understanding the size and scope of operations that can be performed.
 *
 * ------------------------
 *
 * @brief performMatrixOperation:
 * Provides an interface to perform various matrix operations like multiplication or inversion. The actual operations might be dependent on the specific matrix blocks/pages loaded into memory.
 *
 * ------------------------
 *
 * @brief saveMatrix:
 * Writes the matrix (or its current state) back to the `matrixSourceFileName` or to a specified new destination. This ensures persistence of any operations performed.
 *
 * ========================
 */

class Matrix {
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