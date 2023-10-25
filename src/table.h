#include "cursor.h"

enum IndexingStrategy {
    BTREE,
    HASH,
    NOTHING
};

/**
 * ========================
 * TABLE CLASS
 * ========================
 *
 * @brief The Table class holds all information related to a loaded table. It
 * also implements methods that interact with the parsers, executors, cursors
 * and the buffer manager. There are typically 2 ways a table object gets
 * created through the course of the workflow - the first is by using the LOAD
 * command and the second is to use assignment statements (SELECT, PROJECT,
 * JOIN, SORT, CROSS and DISTINCT).
 *
 * Represents a table in a relational database system. Provides functionalities to load, manipulate, and retrieve data, while also handling metadata such as column names and statistics.
 *
 * ------------------------
 *
 * @brief Table::Table:
 * A default constructor that initializes an empty table. Logs the initialization.
 *
 * ------------------------
 *
 * @brief Table::Table(string tableName):
 * Initializes a table object with a given name. The associated data file's name is set based on the table name.
 *
 * ------------------------
 *
 * @brief Table::Table(string tableName, vector<string> columns):
 * Initializes the table with specified columns and calculates block size. Also writes column names to the data file.
 *
 * ------------------------
 *
 * @brief load:
 * Reads data from the source file and divides the data into blocks. Updates table statistics during this process.
 *
 * ------------------------
 *
 * @brief extractColumnNames:
 * Extracts column names from the first line of the .csv file. Checks for any duplicate columns.
 *
 * ------------------------
 *
 * @brief blockify:
 * Reads rows from the source file, divides them into blocks, and writes these blocks to separate files.
 *
 * ------------------------
 *
 * @brief updateStatistics:
 * Given a row, updates the statistics for the table, such as row count and distinct values in each column.
 *
 * ------------------------
 *
 * @brief isColumn:
 * Checks if a given column exists in this table. Returns `true` if present, `false` otherwise.
 *
 * ------------------------
 *
 * @brief renameColumn:
 * Renames a specified column to a new name. Assumes checks for the existence of the old name and non-existence of the new name are done externally.
 *
 * ------------------------
 *
 * @brief print:
 * Prints the first few rows of the table, controlled by a predefined PRINT_COUNT constant.
 *
 * ------------------------
 *
 * @brief getNextPage:
 * Advances the cursor to the next page of the table. This is used for paginated reading of the table data.
 *
 * ------------------------
 *
 * @brief makePermanent:
 * Moves the source file to the "data" folder, making the table permanent.
 *
 * ------------------------
 *
 * @brief isPermanent:
 * Checks if the table's source file is already in the "data" folder, indicating it's a permanent table.
 *
 * ------------------------
 *
 * @brief unload:
 * Removes the table from the database, deleting all associated temporary files.
 *
 * ------------------------
 *
 * @brief getCursor:
 * Returns a cursor for the table, which can be used to read rows sequentially.
 *
 * ------------------------
 *
 * @brief getColumnIndex:
 * Given a column name, returns the index of the column. If not found, behavior is undefined.
 *
 * ------------------------
 *
 * @brief deleteTable:
 * Deletes the table object and its associated data file from the file system. It's a permanent deletion of the table.
 *
 * ------------------------
 *
 * @brief sortComparator:
 * Comparator function to determine the ordering of two vectors a and b based on specified columns and sort orders.
 *
 * Details:
 * - It iterates over the given columns (columnIndexes) and compares the elements of vectors a and b at those indexes.
 * - Uses the sort order (sortValues) to determine the direction of the sort.
 * - If the elements in the current column are equal, it moves to the next column. If they are not, it returns a boolean value based on the sort order.
 *
 * ------------------------
 *
 * @brief MyPair Struct:
 * Represents a custom pair with:
 * - A vector row.
 * - Three integers: blockNum, boundary, and cursorIndex.
 * ------------------------
 *
 * @brief CompareByFirstElement Struct:
 * Comparator struct for ordering the MyPair objects.
 *
 * Details:
 * - Similar to sortComparator, but designed to work with MyPair objects.
 *
 * ------------------------
 *
 * @brief externalSort:
 * Implements an external sort for large data sets that do not fit into memory.
 *
 * Details:
 * - Uses a k-way merging approach to sort the data.
 * - Uses a priority queue (with the custom comparator) to merge the blocks.
 * - Reads and writes blocks of data, sorting them and merging as necessary.
 *
 * ------------------------
 *
 * @brief sortTable:
 * Entry point for sorting a table.
 *
 * Details:
 * - Initializes the sort order (sortValues) and the columns to be sorted (columnIndexes).
 * - Gathers and sorts data page by page. If a page of data is full, it sorts it and then writes it back.
 * - Once all data is sorted page-by-page, it calls externalSort to perform the full external sorting.
 * - Finally, makes the sorted table permanent.
 *
 * ------------------------
 *
 * @brief joinTables:
 * Performs the JOIN operation between two tables based on the given join condition.
 *
 * Details:
 * - Fetches the tables and columns to join based on the parsedQuery.
 * - Creates temporary tables and copies data into them.
 * - Sorts the two tables on the columns to join.
 * - Iterates over the sorted tables and performs the join based on the specified condition.
 * - The resultant joined table is written to disk.
 * - All temporary tables and artifacts are cleaned up.
 *
 * ------------------------
 *
 * @brief orderBy:
 * Sorts the table based on the specified column and order (ASC/DESC).
 *
 * Details:
 * - Creates a new table for the sorted result.
 * - Copies all rows of the current table into the new table.
 * - Sorts the new table based on the specified column and order.
 *
 * @brief groupBy:
 * Implements the GROUP BY operation in SQL, aggregating data based on specified columns.
 *
 * Details:
 * - Fetches required parameters from a global object named `parsedQuery`.
 * - Creates a temporary table for sorting purposes.
 * - Transfers all rows from the current table to this temporary table.
 * - Sorts the temporary table based on the grouping attribute.
 * - Initializes a new table to store grouped results.
 * - Iterates through the sorted rows to apply aggregate functions and grouping.
 * - Writes the aggregated results to pages when they reach a specific size, handling data storage in chunks.
 * - Combines all these pages into a final CSV file to make the grouped table permanent.
 * - Performs clean-up by unloading and deleting temporary tables.
 *
 * =============================
 *
 */

class Table {
    vector<unordered_set<int>> distinctValuesInColumns;

   public:
    string sourceFileName = "";
    string tableName = "";
    vector<string> columns;
    vector<uint> distinctValuesPerColumnCount;
    uint columnCount = 0;
    long long int rowCount = 0;
    uint blockCount = 0;
    uint maxRowsPerBlock = 0;
    vector<uint> rowsPerBlockCount;
    bool indexed = false;
    string indexedColumn = "";
    IndexingStrategy indexingStrategy = NOTHING;

    bool extractColumnNames(string firstLine);
    bool blockify();
    void updateStatistics(vector<int> row);
    Table();
    Table(string tableName);
    Table(string tableName, vector<string> columns);
    bool load();
    bool isColumn(string columnName);
    void renameColumn(string fromColumnName, string toColumnName);
    void print();
    void makePermanent();
    bool isPermanent();
    void getNextPage(Cursor *cursor);
    Cursor getCursor();
    int getColumnIndex(string columnName);
    void deleteTable();
    void unload();
    void sortTable();
    void externalSort();
    void orderBy();
    void joinTables();
    void groupBy();
    /**
     * @brief Static function that takes a vector of valued and prints them out in a
     * comma seperated format.
     *
     * @tparam T current usaages include int and string
     * @param row
     */
    template <typename T>
    void writeRow(vector<T> row, ostream &fout) {
        logger.log("Table::printRow");
        for (int columnCounter = 0; columnCounter < row.size(); columnCounter++) {
            if (columnCounter != 0)
                fout << ", ";
            fout << row[columnCounter];
        }
        fout << endl;
    }

    /**
     * @brief Static function that takes a vector of valued and prints them out in a
     * comma seperated format.
     *
     * @tparam T current usaages include int and string
     * @param row
     */
    template <typename T>
    void writeRow(vector<T> row) {
        logger.log("Table::printRow");
        ofstream fout(this->sourceFileName, ios::app);
        this->writeRow(row, fout);
        fout.close();
    }
};