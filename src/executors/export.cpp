#include "global.h"

/**
 * @brief
 * SYNTAX: EXPORT <relation_name>
 */

bool syntacticParseEXPORT()
{
    logger.log("syntacticParseEXPORT");
    if (tokenizedQuery.size() != 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = EXPORT;
    parsedQuery.exportRelationName = tokenizedQuery[1];
    return true;
}

bool syntacticParseEXPORTMATRIX()
{
    logger.log("syntacticParseEXPORTMATRIX");
    if (tokenizedQuery.size() != 3)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = EXPORTMATRIX;
    parsedQuery.exportMatrixName = tokenizedQuery[2];
    return true;
}

bool semanticParseEXPORT()
{
    logger.log("semanticParseEXPORT");
    // Table should exist
    if (tableCatalogue.isTable(parsedQuery.exportRelationName))
        return true;
    cout << "SEMANTIC ERROR: No such relation exists" << endl;
    return false;
}

bool semanticParseEXPORTMATRIX()
{
    logger.log("semanticParseEXPORT");
    // Table should exist
    if (matrixCatalogue.existMatrix(parsedQuery.exportMatrixName))
        return true;
    cout << "SEMANTIC ERROR: No such matrix exists by name:" << parsedQuery.exportMatrixName << endl;
    return false;
}

void executeEXPORT()
{
    logger.log("executeEXPORT");
    Table *table = tableCatalogue.getTable(parsedQuery.exportRelationName);
    table->makePermanent();
    return;
}

void execute_EXPORT_MATRIX()
{
    logger.log("executeEXPORT");
    Matrix *matrix = matrixCatalogue.getMatrix(parsedQuery.exportMatrixName);
    matrix->makePermanent(matrix->matrixSourceFileName, matrix->matrixName, 0);
    return;
}