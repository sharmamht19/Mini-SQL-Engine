#include "global.h"

//<new_table> <- GROUP BY <grouping_attribute> FROM <table_name> HAVING <aggregate(attribute)> <bin_op> <attribute_value> RETURN <aggregate_func(attribute)>

bool syntacticParseGROUPBY() {
    logger.log("Syntactic Parse GROUPBY");

    if (tokenizedQuery.size() != 13 || tokenizedQuery[2] != "GROUP" || tokenizedQuery[3] != "BY") {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    parsedQuery.queryType = GROUPBY;
    parsedQuery.groupResultRelation = tokenizedQuery[0];
    parsedQuery.groupAttribute = tokenizedQuery[4];
    parsedQuery.groupRelation = tokenizedQuery[6];
    parsedQuery.havingAggregate = "";
    parsedQuery.havingAttribute = "";
    parsedQuery.returnAggregate = "";
    parsedQuery.returnAttribute = "";
    int i = 0;
    for (; i < tokenizedQuery[8].size(); i++) {
        if (tokenizedQuery[8][i] == '(') break;
        parsedQuery.havingAggregate += tokenizedQuery[8][i];
    }
    i++;
    for (; i < tokenizedQuery[8].size() - 1; i++) {
        parsedQuery.havingAttribute += tokenizedQuery[8][i];
    }

    i = 0;
    for (; i < tokenizedQuery[12].size(); i++) {
        if (tokenizedQuery[12][i] == '(') break;
        parsedQuery.returnAggregate += tokenizedQuery[12][i];
    }
    i++;
    for (; i < tokenizedQuery[12].size() - 1; i++) {
        parsedQuery.returnAttribute += tokenizedQuery[12][i];
    }
    // cout<<parsedQuery.havingAggregate<<":"<<parsedQuery.havingAttribute<<endl;
    // cout<<parsedQuery.returnAggregate<<":"<<parsedQuery.returnAttribute<<":"<<endl;

    if (parsedQuery.havingAggregate == "MAX")
        parsedQuery.havingAgg = MAX;
    else if (parsedQuery.havingAggregate == "MIN")
        parsedQuery.havingAgg = MIN;
    else if (parsedQuery.havingAggregate == "AVG")
        parsedQuery.havingAgg = AVG;
    else if (parsedQuery.havingAggregate == "SUM")
        parsedQuery.havingAgg = SUM;
    else if (parsedQuery.havingAggregate == "COUNT")
        parsedQuery.havingAgg = COUNT;
    else {
        cout << "SYNTAX ERROR: Incorrect HAVING aggregate function" << endl;
        return false;
    }
    if (parsedQuery.returnAggregate == "MAX")
        parsedQuery.returnAgg = MAX;
    else if (parsedQuery.returnAggregate == "MIN")
        parsedQuery.returnAgg = MIN;
    else if (parsedQuery.returnAggregate == "AVG")
        parsedQuery.returnAgg = AVG;
    else if (parsedQuery.returnAggregate == "SUM")
        parsedQuery.returnAgg = SUM;
    else if (parsedQuery.havingAggregate == "COUNT")
        parsedQuery.returnAgg = COUNT;
    else {
        cout << "SYNTAX ERROR: Incorrect RETURN aggregate function" << endl;
        return false;
    }
    parsedQuery.havingValue = stoi(tokenizedQuery[10]);
    string binaryOperator = tokenizedQuery[9];
    if (binaryOperator == "<")
        parsedQuery.groupBinaryOperator = LESS_THAN;
    else if (binaryOperator == ">")
        parsedQuery.groupBinaryOperator = GREATER_THAN;
    else if (binaryOperator == ">=" || binaryOperator == "=>")
        parsedQuery.groupBinaryOperator = GEQ;
    else if (binaryOperator == "<=" || binaryOperator == "=<")
        parsedQuery.groupBinaryOperator = LEQ;
    else if (binaryOperator == "==")
        parsedQuery.groupBinaryOperator = EQUAL;
    else if (binaryOperator == "!=")
        parsedQuery.groupBinaryOperator = NOT_EQUAL;
    else {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    logger.log("Exiting Syntactic Parse GROUPBY");
    return true;
}

bool semanticParseGROUPBY() {
    logger.log("semanticParseGROUPBY");

    if (tableCatalogue.isTable(parsedQuery.groupResultRelation)) {
        cout << "SEMANTIC ERROR: Resultant relation already exists" << endl;
        return false;
    }

    if (!tableCatalogue.isTable(parsedQuery.groupRelation)) {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    if (!tableCatalogue.isColumnFromTable(parsedQuery.groupAttribute, parsedQuery.groupRelation)) {
        cout << "SEMANTIC ERROR: Grouping Column doesn't exist in relation" << endl;
        return false;
    }

    if (!tableCatalogue.isColumnFromTable(parsedQuery.havingAttribute, parsedQuery.groupRelation)) {
        cout << "SEMANTIC ERROR: Having Column doesn't exist in relation" << endl;
        return false;
    }

    if (!tableCatalogue.isColumnFromTable(parsedQuery.returnAttribute, parsedQuery.groupRelation)) {
        cout << "SEMANTIC ERROR: Return Column doesn't exist in relation" << endl;
        return false;
    }

    return true;
}

void executeGROUPBY() {
    logger.log("executeGROUPBY");
    Table* table = tableCatalogue.getTable(parsedQuery.groupRelation);
    table->groupBy();
    parsedQuery.clear();
}