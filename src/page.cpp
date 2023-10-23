#include "global.h"
/**
 * @brief Construct a new Page object. Never used as part of the code
 *
 */
Page::Page()
{
    this->pageName = "";
    this->tableName = "";
    this->pageIndex = -1;
    this->rowCount = 0;
    this->columnCount = 0;
    this->rows.clear();
}

/**
 * @brief Construct a new Page:: Page object given the table name and page
 * index. When tables are loaded they are broken up into blocks of BLOCK_SIZE
 * and each block is stored in a different file named
 * "<tablename>_Page<pageindex>". For example, If the Page being loaded is of
 * table "R" and the pageIndex is 2 then the file name is "R_Page2". The page
 * loads the rows (or tuples) into a vector of rows (where each row is a vector
 * of integers).
 *
 * @param tableName
 * @param pageIndex
 */
Page::Page(string tableName, int pageIndex)
{
    logger.log("Page::Page");
    this->tableName = tableName;
    this->pageIndex = pageIndex;
    this->pageName = "../data/temp/" + this->tableName + "_Page" + to_string(pageIndex);
    Table table = *tableCatalogue.getTable(tableName);
    this->columnCount = table.columnCount;
    uint maxRowCount = table.maxRowsPerBlock;
    vector<int> row(columnCount, 0);
    this->rows.assign(maxRowCount, row);

    ifstream fin(pageName, ios::in);
    this->rowCount = table.rowsPerBlockCount[pageIndex];
    int number;
    for (uint rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        for (int columnCounter = 0; columnCounter < columnCount; columnCounter++)
        {
            fin >> number;
            this->rows[rowCounter][columnCounter] = number;
        }
    }
    fin.close();
}

/**
 * @brief Construct a new MatrixPage object.
 *
 */
Page::Page(string matrixName, int rowVal, int colval, vector<vector<int>> data, int rowCount, int colCount)
{
    // cout << "Constructed New Matrix Page with indexes: " << rowVal << " " << colval << endl;
    logger.log("Page::Page in (MatrixPage.cpp)");

    this->tableName = matrixName;
    this->rowCount = rowCount;
    this->columnCount = colCount;
    // this->exactPageRows = exactPageRows;
    // this->exactPageColumns = exactPageColumns;
    this->pageIndex = to_string(rowVal);
    this->colPageIndex = to_string(colval);
    this->rows = data;
    this->pageName = "../data/temp/" + (this->tableName) + "_matrix_page_" + this->pageIndex + "_" + this->colPageIndex;
    // cout << "Constructed New Matrix Page with indexes: " <<  << " " << colval << endl;
}

/**
 * @brief Get row from page indexed by rowIndex
 *
 * @param rowIndex
 * @return vector<int>
 */
vector<int> Page::getRow(int rowIndex)
{
    logger.log("Page::getRow");
    vector<int> result;
    result.clear();
    if (rowIndex >= this->rowCount)
        return result;
    return this->rows[rowIndex];
}

Page::Page(string tableName, int pageIndex, vector<vector<int>> rows, int rowCount)
{
    logger.log("Page::Page");
    this->tableName = tableName;
    this->pageIndex = pageIndex;
    this->rows = rows;
    this->rowCount = rowCount;
    this->columnCount = rows[0].size();
    this->pageName = "../data/temp/" + this->tableName + "_Page" + to_string(pageIndex);
}

/**
 * @brief writes current page contents to file.
 *
 */
void Page::writePage()
{
    logger.log("Page::writePage");
    ofstream fout(this->pageName, ios::trunc);
    for (int rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
        {
            if (columnCounter != 0)
                fout << " ";
            fout << this->rows[rowCounter][columnCounter];
        }
        fout << endl;
    }
    fout.close();
}

vector<vector<int>> Page::getMatrixPageEle()
{
    return this->rows;
}

/**
 * @brief writes current MatrixPage contents to file.
 *
 */
void Page::writeMatrixPage()
{
    logger.log("MatrixPage::writeMatrixPage");
    ofstream fout(this->pageName, ios::trunc);
    for (int r = 0; r < matrixDims; r++)
    {
        for (int c = 0; c < matrixDims; c++)
        {
            fout << this->rows[r][c];
            if (c < matrixDims - 1)
            {
                fout << " ";
            }
        }
        fout << endl;
    }
    fout.close();
}

void settableName(Page& obj, string name){
    obj.tableName = name;
}

string gettableName(Page& obj){
    return obj.tableName;
}

void setpageName(Page& obj, string name){
    obj.pageName = name;
}

string getpageName(Page& obj){
    return obj.pageName;
}

string getpageIndex(Page& obj){
    return obj.pageIndex;
}

string getcolPageIndex(Page& obj){
    return obj.colPageIndex;
}