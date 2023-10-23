#include "global.h"

bool syntacticParseRENAMEMATRIX(){
    logger.log("syntacticParseRENAMEMATRIX");
    if(tokenizedQuery.size()!=4){
        cout<<"SYNATX ERROR"<<endl;
        return false;
    }
    parsedQuery.queryType = RENAMEMATRIX;
    parsedQuery.renameFromMatrix = tokenizedQuery[2];
    parsedQuery.renameToMatrix = tokenizedQuery[3];
    return true;
}


bool semanticParseRENAMEMATRIX(){
    logger.log("semanticParseRENAMEMATRIX");
    string matrix = parsedQuery.renameFromMatrix;
    if(matrixCatalogue.existMatrix(matrix)) return true;
    else{
        cout<<"SEMANTIC ERROR: Matrix" + matrix + "doesn't exists"<<endl;
        return false;
    }
}


void executeRENAMEMATRIX(){
    logger.log("executeRENAMEMATRIX");
    string matrix = parsedQuery.renameFromMatrix;
    Matrix* mat = matrixCatalogue.getMatrix(matrix);
    mat->renameMatrix(parsedQuery.renameToMatrix);
    return;
}