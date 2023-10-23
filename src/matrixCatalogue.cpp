#include "global.h"

bool MatrixCatalogue::existMatrix(string matrix)
{
    logger.log("existMatrix in matrixCatalogue.cpp");
    if (this->catalogue.count(matrix))
        return true;
    return false;
}

Matrix *MatrixCatalogue::getMatrix(string matrix)
{
    if (this->existMatrix(matrix))
        return this->catalogue[matrix];
    Matrix *mat = new Matrix("temp");
    return mat;
}

void MatrixCatalogue::insertMatrix(Matrix *matrix)
{
    logger.log("MatrixCatalogue::insertMatrix");
    this->catalogue[matrix->matrixName] = matrix;
}

void MatrixCatalogue::removeMatrix(string matrixName){
    this->catalogue.erase(matrixName);
}