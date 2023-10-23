#include "global.h"

bool syntacticParse()
{
    logger.log("syntacticParse");
    string possibleQueryType = tokenizedQuery[0];

    if (tokenizedQuery.size() < 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    if (possibleQueryType == "CLEAR")
        return syntacticParseCLEAR();
    else if (possibleQueryType == "INDEX")
        return syntacticParseINDEX();
    else if (possibleQueryType == "LIST")
        return syntacticParseLIST();

        //MATRIX FUNCTIONALITIES.....
    else if(possibleQueryType == "LOAD" && tokenizedQuery.size() == 3 && tokenizedQuery[1] == "MATRIX")
        return syntacticParseMATRIXLOAD();
    else if(possibleQueryType == "TRANSPOSE" && tokenizedQuery[1] == "MATRIX" && tokenizedQuery.size() == 3)
        return syntacticParseTRANSPOSE();
    else if (possibleQueryType == "LOAD" && tokenizedQuery.size() == 3 && tokenizedQuery[1] == "MATRIX")
        return syntacticParseMATRIXLOAD();
    else if(possibleQueryType == "CHECKSYMMETRY" && tokenizedQuery.size() == 2)
        return syntacticParseCHECKSYMMETRY();
    else if(possibleQueryType == "RENAME" && tokenizedQuery[1] == "MATRIX" && tokenizedQuery.size()==4)
        return syntacticParseRENAMEMATRIX();

    else if (possibleQueryType == "LOAD")
        return syntacticParseLOAD();
    else if(possibleQueryType == "PRINT" && tokenizedQuery.size() == 3 && tokenizedQuery[1] == "MATRIX")
        return syntacticParsePRINTMATRIX();
    else if (possibleQueryType == "PRINT")
        return syntacticParsePRINT();
    else if (possibleQueryType == "RENAME")
        return syntacticParseRENAME();
    else if (possibleQueryType == "EXPORT" && tokenizedQuery.size() == 3 && tokenizedQuery[1] == "MATRIX")
        return syntacticParseEXPORTMATRIX();
    else if (possibleQueryType == "EXPORT")
        return syntacticParseEXPORT();
    else if (possibleQueryType == "SOURCE")
        return syntacticParseSOURCE();
    else if(possibleQueryType == "COMPUTE")
        return syntacticParseCOMPUTE();
//  <new_relation_name> <- JOIN <tablename1>, <tablename2> ON <column1> <bin_op> <column2>
//<new_table> <- ORDER BY <attribute> ASC|DESC ON <table_name>
    else if(tokenizedQuery.size()==9 && tokenizedQuery[2]=="JOIN")
        return syntacticParseJOIN();
    else if(tokenizedQuery.size()==8 && tokenizedQuery[2]=="ORDER" && tokenizedQuery[3]=="BY")
        return syntacticParseORDERBY();
//<new_table> <- GROUP BY <grouping_attribute> FROM <table_name> HAVING <aggregate(attribute)> <bin_op> <attribute_value> RETURN <aggregate_func(attribute)>
    else if(tokenizedQuery[2]=="GROUP" && tokenizedQuery[3]=="BY")
        return syntacticParseGROUPBY();
    else
    {
        string resultantRelationName = possibleQueryType;
        if (tokenizedQuery.size() < 3)
        {
            cout << "SYNTAX ERROR" << endl;
            return false;
        }
        possibleQueryType = tokenizedQuery[0];
        if (possibleQueryType == "PROJECT")
            return syntacticParsePROJECTION();
        else if (possibleQueryType == "SELECT")
            return syntacticParseSELECTION();
        else if (possibleQueryType == "JOIN")
            return syntacticParseJOIN();
        else if (possibleQueryType == "CROSS")
            return syntacticParseCROSS();
        else if (possibleQueryType == "DISTINCT")
            return syntacticParseDISTINCT();
        else if (possibleQueryType == "SORT")
            return syntacticParseSORT();
        else
        {
            cout << "SYNTAX ERROR" << endl;
            return false;
        }
    }
    return false;
}

ParsedQuery::ParsedQuery()
{
}

void ParsedQuery::clear()
{
    logger.log("ParseQuery::clear");
    this->queryType = UNDETERMINED;

    this->clearRelationName = "";

    this->crossResultRelationName = "";
    this->crossFirstRelationName = "";
    this->crossSecondRelationName = "";

    this->distinctResultRelationName = "";
    this->distinctRelationName = "";

    this->exportRelationName = "";

    this->indexingStrategy = NOTHING;
    this->indexColumnName = "";
    this->indexRelationName = "";

    this->joinBinaryOperator = NO_BINOP_CLAUSE;
    this->joinResultRelationName = "";
    this->joinFirstRelationName = "";
    this->joinSecondRelationName = "";
    this->joinFirstColumnName = "";
    this->joinSecondColumnName = "";

    this->loadRelationName = "";

    this->printRelationName = "";

    this->projectionResultRelationName = "";
    this->projectionColumnList.clear();
    this->projectionRelationName = "";

    this->renameFromColumnName = "";
    this->renameToColumnName = "";
    this->renameRelationName = "";

    this->selectType = NO_SELECT_CLAUSE;
    this->selectionBinaryOperator = NO_BINOP_CLAUSE;
    this->selectionResultRelationName = "";
    this->selectionRelationName = "";
    this->selectionFirstColumnName = "";
    this->selectionSecondColumnName = "";
    this->selectionIntLiteral = 0;

    this->sortingStrategy = NO_SORT_CLAUSE;
    this->sortResultRelationName = "";
    this->sortColumnName = "";
    this->sortRelationName = "";

    this->sourceFileName = "";

    // New changes from here
    this->loadMatrixName = "";


    BinaryOperator joinBinaryOperator = NO_BINOP_CLAUSE;
    string joinResultRelationName = "";
    string joinFirstRelationName = "";
    string joinSecondRelationName = "";
    string joinFirstColumnName = "";
    string joinSecondColumnName = "";

    //orderby 
    string orderResultRelation = "";
    string orderAttribute = "";
    string orderRelationName = "";

    //GROUP BY 
    string groupResultRelation = "";
    string groupAttribute = "";
    string groupRelation = "";
    string havingAttribute = "";
    string havingAggregate = "";
    string returnAggregate = "";
    string returnAttribute = "";
    int havingValue;
    Aggregate havingAgg = NO_AGGREGATE_FUNCTION;
    Aggregate returnAgg = NO_AGGREGATE_FUNCTION;
    BinaryOperator groupBinaryOperator = NO_BINOP_CLAUSE;

    //MATRIX RELATED.....
    string transposeMatrix = "";
    string symmetryMatrixName = "";
    string matrixPrint = "";
    string renameFromMatrix = "";
    string renameToMatrix = "";

}

/**
 * @brief Checks to see if source file exists. Called when LOAD command is
 * invoked.
 *
 * @param tableName
 * @return true
 * @return false
 */
bool isFileExists(string tableName)
{
    string fileName = "../data/" + tableName + ".csv";
    struct stat buffer;
    return (stat(fileName.c_str(), &buffer) == 0);
}

/**
 * @brief Checks to see if source file exists. Called when SOURCE command is
 * invoked.
 *
 * @param tableName
 * @return true
 * @return false
 */
bool isQueryFile(string fileName)
{
    fileName = "../data/" + fileName + ".ra";
    struct stat buffer;
    return (stat(fileName.c_str(), &buffer) == 0);
}
