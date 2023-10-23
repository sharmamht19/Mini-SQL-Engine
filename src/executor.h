#include"semanticParser.h"

void executeCommand();

void executeCLEAR();
void executeCROSS();
void executeDISTINCT();
void executeEXPORT();
void executeINDEX();
void executeJOIN();
void executeLIST();
void executeLOAD();
void executePRINT();
void executePROJECTION();
void executeRENAME();
void executeSELECTION();
void executeSORT();
void executeSOURCE();
void executeORDERBY();
void executeGROUPBY();
bool evaluateBinOp(int value1, int value2, BinaryOperator binaryOperator);
void printRowCount(int rowCount);

//Add new functions from here
void execute_ON_MATRIX_LOAD();
void executeTRANSPOSE();
void executeCHECKSYMMETRY();
void executeRENAMEMATRIX();
void execute_EXPORT_MATRIX();
void execute_COMPUTE_MATRIX();
void executePRINTMATRIX();
