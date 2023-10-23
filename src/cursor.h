#include "bufferManager.h"
/**
 * @brief The cursor is an important component of the system. To read from a
 * table, you need to initialize a cursor. The cursor reads rows from a page one
 * at a time.
 *
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