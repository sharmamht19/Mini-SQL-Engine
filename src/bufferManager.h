#include "page.h"

/**
 * ========================
 * BUFFER MANAGER METHODS
 * ========================
 * @brief The BufferManager is responsible for reading pages to the main memory.
 * Recall that large files are broken and stored as blocks in the hard disk. The
 * minimum amount of memory that can be read from the disk is a block whose size
 * is indicated by BLOCK_SIZE. within this system we simulate blocks by
 * splitting and storing the file as multiple files each of one BLOCK_SIZE,
 * although this isn't traditionally how it's done. You can alternatively just
 * random access to the point where a block begins within the same
 * file. In this system we assume that the the sizes of blocks and pages are the
 * same.
 *
 * The buffer can hold multiple pages quantified by BLOCK_COUNT. The
 * buffer manager follows the FIFO replacement policy i.e. the first block to be
 * read in is replaced by the new incoming block. This replacement policy should
 * be transparent to the executors i.e. the executor should not know if a block
 * was previously present in the buffer or was read in from the disk.
 *
 * @brief BufferManager:
 * Initializes the BufferManager object, setting up necessary properties for managing data blocks in memory.
 *
 * ------------------------
 *
 * @brief getPage:
 * Retrieves the specified page from the buffer pool. If the page isn't already in the pool, the method will fetch it from disk and insert it into the buffer, potentially ejecting the oldest page if the buffer is full.
 *
 * ------------------------
 *
 * @brief inPool:
 * Determines whether a particular page is present in the buffer pool. This can be essential to avoid redundant fetch operations from disk.
 *
 * ------------------------
 *
 * @brief getFromPool:
 * Directly fetches the desired page from the buffer pool. This function assumes that the page is already in the pool and can lead to an error if the assumption is false.
 *
 * ------------------------
 *
 * @brief insertIntoPool:
 * Inserts a particular page into the buffer pool. If the buffer pool is already at maximum capacity (i.e., holding `BLOCK_COUNT` pages), it ejects the oldest page to make space for the new one.
 *
 * ------------------------
 *
 * @brief writePage:
 * Writes a specified set of data to a page on the disk. This is typically invoked when new data tables are created or updated.
 *
 * ------------------------
 *
 * @brief deleteFile:
 * Deletes a particular file from the disk. This can be handy for cleanup operations or when a table is dropped from the database.
 *
 * ------------------------
 *
 * @brief writeMatrix_Page:
 * Handles the writing operation for matrices, committing the data of a matrix to a storage page. This ensures data persistency for matrix operations.
 *
 * ------------------------
 *
 * @brief readMatrixPage:
 * Reads the content of a matrix page from storage. It returns the matrix as a two-dimensional vector, ensuring the matrix can be processed further in memory.
 *
 * ------------------------
 *
 * @brief insertMatrixIntoPool:
 * Inserts a matrix page into the buffer pool, managing memory similarly to the insertIntoPool method but tailored for matrices.
 *
 * ------------------------
 *
 * @brief getMatrix:
 * Fetches a specific matrix based on its unique identifiers. If the matrix isn't already in the buffer pool, it reads from storage and inserts into the pool.
 *
 * ------------------------
 *
 * @brief clearMainMemory:
 * Purges all pages from the main memory buffer pool, ensuring a clean slate for subsequent operations or to free up memory resources.
 *
 * ========================
 */

class BufferManager {
    deque<Page> pages;
    bool inPool(string pageName);
    Page getFromPool(string pageName);
    Page insertIntoPool(string tableName, int pageIndex);

    Page insertMatrixIntoPool(string matrix, string matrixPage, int mRow, int mCol, vector<vector<int>>& vec, int nRow, int nCol);

   public:
    BufferManager();
    Page getPage(string tableName, int pageIndex);
    // void writePage(string pageName, vector<vector<int>> rows);
    void deleteFile(string tableName, int pageIndex);
    void deleteFile(string fileName);
    void writePage(string tableName, int pageIndex, vector<vector<int>> rows, int rowCount);

    // Add new functions for Matrix operations
    void writeMatrix_Page(string matrixName, int row, int col, vector<vector<int>> data);
    Page getMatrix(string matrix, int rowId, int colId);
    vector<vector<int>> readMatrixPage(string matrixPage);
    void clearMainMemory();
};