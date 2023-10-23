#include "matrix.h"

class MatrixCatalogue{
    unordered_map<string, Matrix*> catalogue;
    public:
        MatrixCatalogue(){}; //constructor
        bool existMatrix(string matrix);
        void insertMatrix(Matrix *matrix);
        void removeMatrix(string matrix);
        Matrix *getMatrix(string matrix);
};