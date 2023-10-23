#include "global.h"

/**
 * @brief 
 * SYNTAX: COMPUTE relation_name
 */
bool syntacticParseCOMPUTE()
{
    logger.log("syntacticParseCOMPUTE");
    if (tokenizedQuery.size() != 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = COMPUTE;
    parsedQuery.computeMatrixName = tokenizedQuery[1];
    return true;
}


bool semanticParseCOMPUTE()
{
    logger.log("semanticParseCOMPUTE");
    if (!matrixCatalogue.existMatrix(parsedQuery.computeMatrixName))
    {
        cout << "SEMANTIC ERROR: Matrix does not exists" << endl;
        return false;
    }
    return true;
}


void execute_COMPUTE_MATRIX()
{
    logger.log("execute_COMPUTE_MATRIX");
    Matrix *matrix = matrixCatalogue.getMatrix(parsedQuery.computeMatrixName);
    matrix->computeOnMatrix();
    return;
}