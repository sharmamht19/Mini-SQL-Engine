#include "global.h"

bool syntacticParseCHECKSYMMETRY(){
    logger.log("syntacticParseCHECKSYMMETRY");
    if(tokenizedQuery.size()!=2){
        cout<<"SYNTAX ERROR"<<endl;
        return false;
    }
    parsedQuery.queryType = CHECKSYMMETRY;
    parsedQuery.symmetryMatrixName = tokenizedQuery[1];
    return true;
}

bool semanticParseCHECKSYMMETRY(){
    logger.log("semanticParseCHECKSYMMETRY");
    string matrix = parsedQuery.symmetryMatrixName;
    if(matrixCatalogue.existMatrix(matrix)) return true;
    else{
        cout<<"SEMANTIC ERROR: Matrix "+ matrix + " doesn't exist"<<endl;
        return false;
    }
}

void executeCHECKSYMMETRY(){
    logger.log("executeCHECKSYMMETRY");
    string matrix = parsedQuery.symmetryMatrixName;
    Matrix* mat = matrixCatalogue.getMatrix(matrix);
    mat->doCheckSymmetry();
    return;
}

