#include "bufferManager.h"
/**
 * ========================
 * CURSOR CLASS METHODS
 * ========================
 * The cursor is an important component of the system. To read from a
 * table, you need to initialize a cursor. The cursor reads rows from a page one
 * at a time.
 *
 * @brief Cursor (overloaded for table):
 * Initializes a cursor for reading from a table, setting the specified table and page. This cursor allows sequential access to the rows stored in a given page of the table.
 *
 * ------------------------
 *
 * @brief Cursor (overloaded for matrix):
 * Initializes a cursor specifically tailored for matrices. Similar to the table-focused constructor, but includes an additional column page index for matrix operations.
 *
 * ------------------------
 *
 * @brief operator=:
 * Allows deep copying of a cursor object. This ensures that when one cursor is assigned to another, all the internal attributes (like page details and pointers) are properly copied.
 *
 * ------------------------
 *
 * @brief getNext:
 * Reads and returns the next row from the current page in the cursor. The position of the current row in the page is determined by the `pagePointer`. If the end of the current page is reached, it fetches the next page for continuity.
 *
 * ------------------------
 *
 * @brief getNextPageRow:
 * This is similar to `getNext`, but it doesn't automatically fetch the next page when the end of the current page is reached.
 *
 * ------------------------
 *
 * @brief getMatrixPageRow:
 * Retrieves a specific row from a matrix page. This is based on the specified row index and doesn't require sequential access like the other `getNext` methods.
 *
 * ------------------------
 *
 * @brief nextPage:
 * Sets the cursor to start reading from a new page. This allows direct navigation to any page in a table, resetting the `pagePointer` to the beginning of the new page.
 *
 * ========================
 */

class Cursor {
   public:
    Page page;
    int pageIndex;
    int colPageIndex;
    string tableName;
    int pagePointer;

   public:
    Cursor(string tableName, int pageIndex);
    Cursor(string matrixName, int pageRowID, int pageColID);
    vector<int> getNext();
    vector<int> getNextPageRow();
    vector<int> getNextMatrixRow();
    vector<int> getMatrixPageRow(int rowId);
    void nextPage(int pageIndex);
    Cursor& operator=(const Cursor& other);
};