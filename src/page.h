#include "logger.h"

/**
 * ========================
 * PAGE CLASS
 * ========================
 *
 * @brief The Page object is the main memory representation of a physical page
 * (equivalent to a block). The page class and the page.h header file are at the
 * bottom of the dependency tree when compiling files.
 *
 * Represents a memory block or "page" containing data, which can either be part of a table or a matrix. Each page corresponds to a block of data stored in a file, with functionalities to read from and write back to these files.
 *
 * ------------------------
 *
 * @brief Page (Default Constructor):
 * Constructs an empty page with no association to any table or matrix and initializes all basic attributes to their default values.
 *
 * ------------------------
 *
 * @brief Page (Overloaded for tables):
 * Given the table name and page index, constructs a new page by reading data from the corresponding file, breaking the table into blocks of size BLOCK_SIZE. This constructor is tailored for table data.
 *
 * ------------------------
 *
 * @brief Page (Overloaded for matrix data):
 * Constructs a matrix page using provided matrix name, row and column indices, and other matrix-related data. The matrix data is then divided based on matrix dimensions, and each block of the matrix is treated as a page.
 *
 * ------------------------
 *
 * @brief Page (Another overloaded version for tables):
 * Constructs a page for a table with a specified row data and page index. Useful for creating a page in memory without reading from a file.
 *
 * ------------------------
 *
 * @brief getRow:
 * Given a row index, fetches and returns the corresponding row from the current page.
 *
 * ------------------------
 *
 * @brief writePage:
 * Writes the content of the current page (whether table or matrix data) to the associated file.
 *
 * ------------------------
 *
 * @brief getMatrixPageEle:
 * For matrix pages, retrieves the complete data or all rows stored in the current matrix page.
 *
 * ------------------------
 *
 * @brief writeMatrixPage:
 * Specifically for matrix pages, writes the current content back to its file. This ensures that matrix data is saved in a format that aligns with matrix dimensions.
 *
 * ------------------------
 *
 * @brief settableName:
 * Updates the name of the table associated with the page.
 *
 * ------------------------
 *
 * @brief gettableName:
 * Fetches the name of the table that the current page is associated with.
 *
 * ------------------------
 *
 * @brief setpageName:
 * Updates the page's filename, which is where the page data will be read from or written to.
 *
 * ------------------------
 *
 * @brief getpageName:
 * Fetches the filename corresponding to the current page.
 *
 * ------------------------
 *
 * @brief getpageIndex:
 * Retrieves the index of the current page, which can help in navigating multi-page tables or matrices.
 *
 * ------------------------
 *
 * @brief getcolPageIndex:
 * For matrix pages, fetches the column index of the current matrix block.
 *
 * ========================
 */

class Page {
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