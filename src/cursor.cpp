#include "global.h"

Cursor::Cursor(string tableName, int pageIndex) {
    logger.log("Cursor::Cursor");
    this->page = bufferManager.getPage(tableName, pageIndex);
    this->pagePointer = 0;
    this->tableName = tableName;
    this->pageIndex = pageIndex;
}

Cursor::Cursor(string matrixName, int pageIndex, int colPageIndex) {
    logger.log("Cursor::Cursor");
    this->page = bufferManager.getMatrix(matrixName, pageIndex, colPageIndex);
    this->pagePointer = 0;
    this->tableName = matrixName;
    this->pageIndex = pageIndex;
    this->colPageIndex = colPageIndex;
}

// Deep Copy of a cursor..
Cursor& Cursor::operator=(const Cursor& other) {
    if (this == &other) {
        return *this;  // Self-assignment, no need to copy
    }

    // Copy the page, pagePointer, tableName, pageIndex, and colPageIndex
    this->page = other.page;
    this->pagePointer = other.pagePointer;
    this->tableName = other.tableName;
    this->pageIndex = other.pageIndex;
    this->colPageIndex = other.colPageIndex;

    return *this;
}

/**
 * @brief This function reads the next row from the page. The index of the
 * current row read from the page is indicated by the pagePointer(points to row
 * in page the cursor is pointing to).
 *
 * @return vector<int>
 */
vector<int> Cursor::getNext() {
    logger.log("Cursor::geNext");
    vector<int> result = this->page.getRow(this->pagePointer);
    this->pagePointer++;
    if (result.empty()) {
        tableCatalogue.getTable(this->tableName)->getNextPage(this);
        if (!this->pagePointer) {
            result = this->page.getRow(this->pagePointer);
            this->pagePointer++;
        }
    }
    return result;
}

/**
 * @brief This function reads the next row from the page. The index of the
 * current row read from the page is indicated by the pagePointer(points to row
 * in page the cursor is pointing to).
 *
 * @return vector<int>
 */
vector<int> Cursor::getNextPageRow() {
    logger.log("Cursor::geNext");
    vector<int> result = this->page.getRow(this->pagePointer);
    this->pagePointer++;
    // if (result.empty())
    // {
    //     tableCatalogue.getTable(this->tableName)->getNextPage(this);
    //     if (!this->pagePointer)
    //     {
    //         result = this->page.getRow(this->pagePointer);
    //         this->pagePointer++;
    //     }
    // }
    return result;
}

/**
 * @brief This function reads the next row from the page. The index of the
 * current row read from the page is indicated by the pagePointer(points to row
 * in page the cursor is pointing to).
 *
 * @return vector<int>
 */
vector<int> Cursor::getMatrixPageRow(int rowIndex) {
    logger.log("Cursor::geNext");
    this->pagePointer = rowIndex;
    vector<int> result = this->page.getRow(this->pagePointer);
    // this->pagePointer++;
    // if(result.empty()){
    //     tableCatalogue.getTable(this->tableName)->getNextPage(this);
    //     if(!this->pagePointer){
    //         result = this->page.getRow(this->pagePointer);
    //         this->pagePointer++;
    //     }
    // }
    return result;
}

/**
 * @brief Function that loads Page indicated by pageIndex. Now the cursor starts
 * reading from the new page.
 *
 * @param pageIndex
 */
void Cursor::nextPage(int pageIndex) {
    logger.log("Cursor::nextPage");
    this->page = bufferManager.getPage(this->tableName, pageIndex);
    this->pageIndex = pageIndex;
    this->pagePointer = 0;
}