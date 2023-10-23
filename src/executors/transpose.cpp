#include "global.h"

bool syntacticParseTRANSPOSE(){
    logger.log("syntacticParseTRANSPOSE");
    if(tokenizedQuery.size()!=3){
        cout<<"SYNTAX ERROR"<<endl;
        return false;
    }
    parsedQuery.queryType = TRANSPOSE;
    parsedQuery.transposeMatrix = tokenizedQuery[2];
    return true;
}

bool semanticParseTRANSPOSE(){
    logger.log("semanticParseTRANSPOSE");
    //logic for semantic parsing.....
    string transposeMatrix = parsedQuery.transposeMatrix;
    if(matrixCatalogue.existMatrix(transposeMatrix)) return true;
    else{
        cout<<"SEMANTIC ERROR: Matrix "+ transposeMatrix + " needed for transpose does not exist"<<endl;
        return false;
    }
}

void executeTRANSPOSE(){
    logger.log("executeTRANSPOSE");
    string transposeMatrix = parsedQuery.transposeMatrix;
    //get the matrix from the catalogue and then transpose it.
    Matrix *mat = matrixCatalogue.getMatrix(transposeMatrix);
    mat->doTranspose();
    return;
}