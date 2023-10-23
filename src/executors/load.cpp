#include "global.h"

/**
 * @brief
 * SYNTAX: LOAD relation_name
 */
bool syntacticParseLOAD()
{
    logger.log("syntacticParseLOAD");
    if (tokenizedQuery.size() != 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = LOAD;
    parsedQuery.loadRelationName = tokenizedQuery[1];
    return true;
}

bool semanticParseLOAD()
{
    logger.log("semanticParseLOAD");
    if (tableCatalogue.isTable(parsedQuery.loadRelationName))
    {
        cout << "SEMANTIC ERROR: Relation already exists" << endl;
        return false;
    }

    if (!isFileExists(parsedQuery.loadRelationName))
    {
        cout << "SEMANTIC ERROR: Data file doesn't exist" << endl;
        return false;
    }
    return true;
}

void executeLOAD()
{
    logger.log("executeLOAD");

    Table *table = new Table(parsedQuery.loadRelationName);
    if (table->load())
    {
        tableCatalogue.insertTable(table);
        cout << "Loaded Table. Column Count: " << table->columnCount << " Row Count: " << table->rowCount << endl;
    }
    return;
}

bool syntacticParseMATRIXLOAD()
{
    logger.log("syntacticParseMATRIXLOAD");
    if (tokenizedQuery.size() != 3)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = LOADMATRIX;
    parsedQuery.loadMatrixName = tokenizedQuery[2];
    return true;
}

bool semanticParseMATRIXLOAD()
{
    if (!isFileExists(parsedQuery.loadMatrixName))
    {
        cout << "SEMANTIC ERROR: Data file doesn't exist" << endl;
        return false;
    }
    if (tableCatalogue.isTable(parsedQuery.loadRelationName))
    {
        cout << "SEMANTIC ERROR: Matrix already exists" << endl;
        return false;
    }
    return true;
}

void execute_ON_MATRIX_LOAD()
{
    logger.log("Executing LOAD MATRIX CMD (in load.cpp)");
    Matrix *mat = new Matrix(parsedQuery.loadMatrixName);
    if (mat->load())
    {
        matrixCatalogue.insertMatrix(mat);
        cout << "Load Successfull" << endl;
        logger.log("Executed LOAD MATRIX CMD (in load.cpp)");
    }
    else
    {
        cout << "Load unsuccessfull" << endl;
        logger.log("Failed LOAD MATRIX CMD (in load.cpp)");
    }
}