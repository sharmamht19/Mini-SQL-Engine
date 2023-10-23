#include "tableCatalogue.h"
// #include "matrix.h"
#include "matrixCatalogue.h"
using namespace std;

enum QueryType
{
    CLEAR,
    CROSS,
    DISTINCT,
    EXPORT,
    INDEX,
    JOIN,
    LIST,
    LOAD,
    PRINT,
    PROJECTION,
    RENAME,
    SELECTION,
    SORT,
    SOURCE,
    UNDETERMINED,
    //New Cmds from here
    LOADMATRIX,
    TRANSPOSE,
    CHECKSYMMETRY,
    RENAMEMATRIX,
    EXPORTMATRIX,
    COMPUTE,
    PRINTMATRIX,
    ORDERBY,
    GROUPBY
};

enum BinaryOperator
{
    LESS_THAN,
    GREATER_THAN,
    LEQ,
    GEQ,
    EQUAL,
    NOT_EQUAL,
    NO_BINOP_CLAUSE
};

enum SortingStrategy
{
    ASC,
    DESC,
    NO_SORT_CLAUSE
};

enum SelectType
{
    COLUMN,
    INT_LITERAL,
    NO_SELECT_CLAUSE
};

enum Aggregate
{
    MAX,
    MIN,
    SUM,
    AVG,
    COUNT,
    NO_AGGREGATE_FUNCTION
};

class ParsedQuery
{

public:
    QueryType queryType = UNDETERMINED;

    string clearRelationName = "";

    string crossResultRelationName = "";
    string crossFirstRelationName = "";
    string crossSecondRelationName = "";

    string distinctResultRelationName = "";
    string distinctRelationName = "";

    string exportRelationName = "";

    IndexingStrategy indexingStrategy = NOTHING;
    string indexColumnName = "";
    string indexRelationName = "";

    BinaryOperator joinBinaryOperator = NO_BINOP_CLAUSE;
    string joinResultRelationName = "";
    string joinFirstRelationName = "";
    string joinSecondRelationName = "";
    string joinFirstColumnName = "";
    string joinSecondColumnName = "";

    string loadRelationName = "";

    string printRelationName = "";

    string projectionResultRelationName = "";
    vector<string> projectionColumnList;
    string projectionRelationName = "";

    string renameFromColumnName = "";
    string renameToColumnName = "";
    string renameRelationName = "";

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
    int havingValue = 0;
    Aggregate havingAgg = NO_AGGREGATE_FUNCTION;
    Aggregate returnAgg = NO_AGGREGATE_FUNCTION;
    BinaryOperator groupBinaryOperator = NO_BINOP_CLAUSE;

    //MATRIX RELATED.....
    string transposeMatrix = "";
    string symmetryMatrixName = "";
    string matrixPrint = "";
    string renameFromMatrix = "";
    string renameToMatrix = "";

    SelectType selectType = NO_SELECT_CLAUSE;
    BinaryOperator selectionBinaryOperator = NO_BINOP_CLAUSE;
    string selectionResultRelationName = "";
    string selectionRelationName = "";
    string selectionFirstColumnName = "";
    string selectionSecondColumnName = "";
    int selectionIntLiteral = 0;

    SortingStrategy sortingStrategy = NO_SORT_CLAUSE;
    string sortResultRelationName = "";
    string sortColumnName = "";
    vector<string> sortColumns;
    vector<SortingStrategy> sortStrategy;
    string sortRelationName = "";

    string sourceFileName = "";

    //Variable that stores matrix name given with LOAD MATRIX cmd
    string loadMatrixName = "";
    //Variable that stores matrix name given with EXPORT MATRIX cmd
    string exportMatrixName = "";
    //Variable that stores matrix name given with COMPUTE cmd
    string computeMatrixName = "";
    //Variable that stores matrix name given with PRINT MATRIX cmd
    string printMatrixName = "";
    ParsedQuery();
    void clear();
};

bool syntacticParse();
bool syntacticParseCLEAR();
bool syntacticParseCROSS();
bool syntacticParseDISTINCT();
bool syntacticParseEXPORT();
bool syntacticParseINDEX();
bool syntacticParseJOIN();
bool syntacticParseLIST();
bool syntacticParseLOAD();
bool syntacticParsePRINT();
bool syntacticParsePROJECTION();
bool syntacticParseRENAME();
bool syntacticParseSELECTION();
bool syntacticParseSORT();
bool syntacticParseSOURCE();
bool syntacticParseORDERBY();
bool syntacticParseGROUPBY();

//Check if LOAD MATRIX CMD is syntatically correct
bool syntacticParseMATRIXLOAD();
bool syntacticParseTRANSPOSE();
bool syntacticParseCHECKSYMMETRY();
bool syntacticParseRENAMEMATRIX();


bool syntacticParseEXPORTMATRIX();
bool syntacticParseCOMPUTE();
bool syntacticParsePRINTMATRIX();
bool isFileExists(string tableName);
bool isQueryFile(string fileName);