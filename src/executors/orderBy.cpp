#include "global.h"

// <new_table> <- ORDER BY <attribute> ASC|DESC ON <table_name>

bool syntacticParseORDERBY(){
    logger.log("syntacticParseORDERBY()");
    if(tokenizedQuery.size()!=8 && tokenizedQuery[2]!="ORDER" && tokenizedQuery[3]!="BY"){
        cout<<"SYNATX ERROR"<<endl;
        return false;
    }
    parsedQuery.queryType = ORDERBY;
    parsedQuery.orderResultRelation = tokenizedQuery[0];
    parsedQuery.orderRelationName = tokenizedQuery[7];
    parsedQuery.orderAttribute = tokenizedQuery[4];
    string sortingStrategy = tokenizedQuery[5];
    if(sortingStrategy == "ASC")
        parsedQuery.sortingStrategy = ASC;
    else if(sortingStrategy == "DESC")
        parsedQuery.sortingStrategy = DESC;
    else{
        cout<<"SYNTAX ERROR"<<endl;
        return false;
    }
    return true;
}


bool semanticParseORDERBY(){
    logger.log("semanticParseORDERBY");

    if(tableCatalogue.isTable(parsedQuery.orderResultRelation)){
        cout<<"SEMANTIC ERROR: Resultant relation already exists"<<endl;
        return false;
    }

    if(!tableCatalogue.isTable(parsedQuery.orderRelationName)){
        cout<<"SEMANTIC ERROR: Relation doesn't exist"<<endl;
        return false;
    }

    if(!tableCatalogue.isColumnFromTable(parsedQuery.orderAttribute, parsedQuery.orderRelationName)){
        cout<<"SEMANTIC ERROR: Column doesn't exist in relation"<<endl;
        return false;
    }

    return true;
}


void executeORDERBY(){
    logger.log("executeORDERBY");
    Table* table = tableCatalogue.getTable(parsedQuery.orderRelationName);
    table->orderBy();
    parsedQuery.clear();
}