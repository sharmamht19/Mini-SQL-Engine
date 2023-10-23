#include "logger.h"
/**
 * @brief The Page object is the main memory representation of a physical page
 * (equivalent to a block). The page class and the page.h header file are at the
 * bottom of the dependency tree when compiling files.
 *<p>
 * Do NOT modify the Page class. If you find that modifications
 * are necessary, you may do so by posting the change you want to make on Moodle
 * or Teams with justification and gaining approval from the TAs.
 *</p>
 */

class Page
{
    string tableName;
    string pageIndex;
    string colPageIndex;
    int columnCount;
    int rowCount;
    int exactPageRows;
    int exactPageColumns;
    vector<vector<int>> rows;

public:
    string pageName = "";
    Page();
    Page(string tableName, int pageIndex);
    Page(string tableName, int pageIndex, vector<vector<int>> rows, int rowCount);
    Page(string matrixName, int rowVal, int colval, vector<vector<int>> data, int rowCount, int colCount);
    vector<int> getRow(int rowIndex);
    void writePage();
    void writeMatrixPage();
    vector<vector<int>> getMatrixPageEle();
    friend void settableName(Page& obj, string name);
    friend string gettableName(Page& obj);
    friend void setpageName(Page& obj, string name);
    friend string getpageName(Page& obj);
    friend string getpageIndex(Page& obj);
    friend string getcolPageIndex(Page& obj);
};
void settableName(Page& obj, string name);
string gettableName(Page& obj);
void setpageName(Page& obj, string name);
string getpageName(Page& obj);
string getpageIndex(Page& obj);
string getcolPageIndex(Page& obj);
// class MatrixPage : public Page
// {
// public:
//     int rowNum, colNum;
//     string matrix_Name;
//     // string matrix_file_loc;
//     // vector<vector<int>> matrixData;
//     MatrixPage();
//     MatrixPage(string matrixName, int rowVal, int colval, vector<vector<int>> data);
//     void writeMatrixPage();
// };