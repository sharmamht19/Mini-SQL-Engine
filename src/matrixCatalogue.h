#include "matrix.h"

/**
 * ========================
 * MATRIXCATALOGUE CLASS
 * ========================
 *
 * A class to manage matrices in a catalogue. It offers functionalities to check the existence of a matrix, retrieve it, insert a new matrix, and remove an existing matrix.
 *
 * ------------------------
 *
 * @brief existMatrix:
 * Checks if a specified matrix exists within the catalogue.
 *
 * ------------------------
 *
 * @brief getMatrix:
 * Fetches the specified matrix from the catalogue or returns a temporary matrix if not found.
 *
 * ------------------------
 *
 * @brief insertMatrix:
 * Inserts a given matrix into the catalogue.
 *
 * ------------------------
 *
 * @brief removeMatrix:
 * Removes a matrix from the catalogue using its name.
 *
 * ========================
 */

class MatrixCatalogue {
    unordered_map<string, Matrix *> catalogue;

   public:
    MatrixCatalogue(){};  // constructor
    bool existMatrix(string matrix);
    void insertMatrix(Matrix *matrix);
    void removeMatrix(string matrix);
    Matrix *getMatrix(string matrix);
};