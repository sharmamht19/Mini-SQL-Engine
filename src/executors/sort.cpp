#include<global.h>

bool syntacticParseSORT(){
    logger.log("syntacticParseSORT");
    int n = tokenizedQuery.size();
    bool in = false;
    int ind=-1;
    for(int i=0;i<n;i++){
        if(tokenizedQuery[i]=="IN"){
            in = true;
            ind = i;
            break;
        }
    }
    if(tokenizedQuery[2]!="BY"){
        cout<<"SYNATX ERROR 1"<<endl;
        return false;
    }
    parsedQuery.queryType = SORT;
    parsedQuery.sortRelationName = tokenizedQuery[1];
    int i = 3;
    while(tokenizedQuery[i]!="IN"){
        parsedQuery.sortColumns.push_back(tokenizedQuery[i]);
        i++;
    }
    i++;
    vector<string> sortingStrategy;
    while(i<n){
        sortingStrategy.push_back(tokenizedQuery[i]);
        i++;
    }
    for(int i=0;i<sortingStrategy.size();i++){
        if(sortingStrategy[i] == "ASC")
            parsedQuery.sortStrategy.push_back(ASC);
        else if(sortingStrategy[i] == "DESC")
            parsedQuery.sortStrategy.push_back(DESC);
        else{
            cout<<"SYNTAX ERROR 2"<<endl;
            return false;
        }   
    }
    return true;
}

bool semanticParseSORT(){
    logger.log("semanticParseSORT");

    if(!tableCatalogue.isTable(parsedQuery.sortRelationName)){
        cout<<"SEMANTIC ERROR: Relation doesn't exist"<<endl;
        return false;
    }
    int n = parsedQuery.sortColumns.size();
    for(int i=0;i<n;i++){
        string sortColumnName = parsedQuery.sortColumns[i];
        if(!tableCatalogue.isColumnFromTable(sortColumnName, parsedQuery.sortRelationName)){
            cout<<"SEMANTIC ERROR: Column doesn't exist in relation"<<endl;
            return false;
        }
    }

    return true;
}

void executeSORT(){
    logger.log("executeSORT");
    Table* table = tableCatalogue.getTable(parsedQuery.sortRelationName);
    table->sortTable();
    // table->externalSort(parsedQuery.sortColumns, parsedQuery.sortStrategy);
}