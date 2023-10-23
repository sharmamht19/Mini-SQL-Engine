#include"global.h"

void executeCommand(){

    switch(parsedQuery.queryType){
        case CLEAR: executeCLEAR(); break;
        case CROSS: executeCROSS(); break;
        case DISTINCT: executeDISTINCT(); break;
        case EXPORT: executeEXPORT(); break;
        case INDEX: executeINDEX(); break;
        case JOIN: executeJOIN(); break;
        case LIST: executeLIST(); break;
        case LOAD: executeLOAD(); break;
        case PRINT: executePRINT(); break;
        case PROJECTION: executePROJECTION(); break;
        case RENAME: executeRENAME(); break;
        case SELECTION: executeSELECTION(); break;
        case SORT: executeSORT(); break;
        case SOURCE: executeSOURCE(); break;
        case ORDERBY: executeORDERBY(); break;
        case GROUPBY: executeGROUPBY(); break;
        //Add new functions from here
        case LOADMATRIX: execute_ON_MATRIX_LOAD();break;
        case TRANSPOSE: executeTRANSPOSE(); break;
        case CHECKSYMMETRY: executeCHECKSYMMETRY(); break;
        case RENAMEMATRIX: executeRENAMEMATRIX(); break;
        case EXPORTMATRIX: execute_EXPORT_MATRIX(); break;
        case COMPUTE: execute_COMPUTE_MATRIX(); break;
        case PRINTMATRIX: executePRINTMATRIX(); break;
        default: cout<<"PARSING ERROR"<<endl;
    }

    return;
}

void printRowCount(int rowCount){
    cout<<"\n\nRow Count: "<<rowCount<<endl;
    return;
}