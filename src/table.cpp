#include "global.h"

vector<int> sortValues, columnIndexes;

/**
 * @brief Construct a new Table:: Table object
 *
 */
Table::Table() {
    logger.log("Table::Table");
}

/**
 * @brief Construct a new Table:: Table object used in the case where the data
 * file is available and LOAD command has been called. This command should be
 * followed by calling the load function;
 *
 * @param tableName
 */
Table::Table(string tableName) {
    logger.log("Table::Table");
    this->sourceFileName = "../data/" + tableName + ".csv";
    this->tableName = tableName;
}

/**
 * @brief Construct a new Table:: Table object used when an assignment command
 * is encountered. To create the table object both the table name and the
 * columns the table holds should be specified.
 *
 * @param tableName
 * @param columns
 */
Table::Table(string tableName, vector<string> columns) {
    logger.log("Table::Table");
    this->sourceFileName = "../data/" + tableName + ".csv";
    this->tableName = tableName;
    this->columns = columns;
    this->columnCount = columns.size();
    this->maxRowsPerBlock = (uint)((BLOCK_SIZE * 1024) / (sizeof(int) * columnCount));
    this->writeRow<string>(columns);
}

/**
 * @brief The load function is used when the LOAD command is encountered. It
 * reads data from the source file, splits it into blocks and updates table
 * statistics.
 *
 * @return true if the table has been successfully loaded
 * @return false if an error occurred
 */
bool Table::load() {
    logger.log("Table::load");
    fstream fin(this->sourceFileName, ios::in);
    string line;
    if (getline(fin, line)) {
        fin.close();
        if (this->extractColumnNames(line))
            if (this->blockify())
                return true;
    }
    fin.close();
    return false;
}

/**
 * @brief Function extracts column names from the header line of the .csv data
 * file.
 *
 * @param line
 * @return true if column names successfully extracted (i.e. no column name
 * repeats)
 * @return false otherwise
 */
bool Table::extractColumnNames(string firstLine) {
    logger.log("Table::extractColumnNames");
    unordered_set<string> columnNames;
    string word;
    stringstream s(firstLine);
    while (getline(s, word, ',')) {
        word.erase(std::remove_if(word.begin(), word.end(), ::isspace), word.end());
        if (columnNames.count(word))
            return false;
        columnNames.insert(word);
        this->columns.emplace_back(word);
    }
    this->columnCount = this->columns.size();
    this->maxRowsPerBlock = (uint)((BLOCK_SIZE * 1024) / (sizeof(int) * this->columnCount));
    return true;
}

/**
 * @brief This function splits all the rows and stores them in multiple files of
 * one block size.
 *
 * @return true if successfully blockified
 * @return false otherwise
 */
bool Table::blockify() {
    logger.log("Table::blockify");
    ifstream fin(this->sourceFileName, ios::in);
    string line, word;
    vector<int> row(this->columnCount, 0);
    vector<vector<int>> rowsInPage(this->maxRowsPerBlock, row);
    int pageCounter = 0;
    unordered_set<int> dummy;
    dummy.clear();
    this->distinctValuesInColumns.assign(this->columnCount, dummy);
    this->distinctValuesPerColumnCount.assign(this->columnCount, 0);
    getline(fin, line);
    while (getline(fin, line)) {
        stringstream s(line);
        for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++) {
            if (!getline(s, word, ','))
                return false;
            row[columnCounter] = stoi(word);
            rowsInPage[pageCounter][columnCounter] = row[columnCounter];
        }
        pageCounter++;
        this->updateStatistics(row);
        if (pageCounter == this->maxRowsPerBlock) {
            bufferManager.writePage(this->tableName, this->blockCount, rowsInPage, pageCounter);
            this->blockCount++;
            this->rowsPerBlockCount.emplace_back(pageCounter);
            pageCounter = 0;
        }
    }
    if (pageCounter) {
        bufferManager.writePage(this->tableName, this->blockCount, rowsInPage, pageCounter);
        this->blockCount++;
        this->rowsPerBlockCount.emplace_back(pageCounter);
        pageCounter = 0;
    }

    if (this->rowCount == 0)
        return false;
    this->distinctValuesInColumns.clear();
    return true;
}

/**
 * @brief Given a row of values, this function will update the statistics it
 * stores i.e. it updates the number of rows that are present in the column and
 * the number of distinct values present in each column. These statistics are to
 * be used during optimisation.
 *
 * @param row
 */
void Table::updateStatistics(vector<int> row) {
    this->rowCount++;
    for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++) {
        if (!this->distinctValuesInColumns[columnCounter].count(row[columnCounter])) {
            this->distinctValuesInColumns[columnCounter].insert(row[columnCounter]);
            this->distinctValuesPerColumnCount[columnCounter]++;
        }
    }
}

/**
 * @brief Checks if the given column is present in this table.
 *
 * @param columnName
 * @return true
 * @return false
 */
bool Table::isColumn(string columnName) {
    logger.log("Table::isColumn");
    for (auto col : this->columns) {
        if (col == columnName) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Renames the column indicated by fromColumnName to toColumnName. It is
 * assumed that checks such as the existence of fromColumnName and the non prior
 * existence of toColumnName are done.
 *
 * @param fromColumnName
 * @param toColumnName
 */
void Table::renameColumn(string fromColumnName, string toColumnName) {
    logger.log("Table::renameColumn");
    for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++) {
        if (columns[columnCounter] == fromColumnName) {
            columns[columnCounter] = toColumnName;
            break;
        }
    }
    return;
}

/**
 * @brief Function prints the first few rows of the table. If the table contains
 * more rows than PRINT_COUNT, exactly PRINT_COUNT rows are printed, else all
 * the rows are printed.
 *
 */
void Table::print() {
    logger.log("Table::print");
    uint count = min((long long)PRINT_COUNT, this->rowCount);

    // print headings
    this->writeRow(this->columns, cout);

    Cursor cursor(this->tableName, 0);
    vector<int> row;
    for (int rowCounter = 0; rowCounter < count; rowCounter++) {
        row = cursor.getNext();
        this->writeRow(row, cout);
    }
    printRowCount(this->rowCount);
}

/**
 * @brief This function returns one row of the table using the cursor object. It
 * returns an empty row is all rows have been read.
 *
 * @param cursor
 * @return vector<int>
 */
void Table::getNextPage(Cursor *cursor) {
    logger.log("Table::getNext");

    if (cursor->pageIndex < this->blockCount - 1) {
        cursor->nextPage(cursor->pageIndex + 1);
    }
}

/**
 * @brief called when EXPORT command is invoked to move source file to "data"
 * folder.
 *
 */
void Table::makePermanent() {
    logger.log("Table::makePermanent");
    if (!this->isPermanent())
        bufferManager.deleteFile(this->sourceFileName);
    string newSourceFile = "../data/" + this->tableName + ".csv";
    ofstream fout(newSourceFile, ios::out);

    // print headings
    this->writeRow(this->columns, fout);

    Cursor cursor(this->tableName, 0);
    vector<int> row;
    for (int rowCounter = 0; rowCounter < this->rowCount; rowCounter++) {
        row = cursor.getNext();
        this->writeRow(row, fout);
    }
    fout.close();
}

/**
 * @brief Function to check if table is already exported
 *
 * @return true if exported
 * @return false otherwise
 */
bool Table::isPermanent() {
    logger.log("Table::isPermanent");
    if (this->sourceFileName == "../data/" + this->tableName + ".csv")
        return true;
    return false;
}

/**
 * @brief The unload function removes the table from the database by deleting
 * all temporary files created as part of this table
 *
 */
void Table::unload() {
    logger.log("Table::~unload");
    for (int pageCounter = 0; pageCounter < this->blockCount; pageCounter++)
        bufferManager.deleteFile(this->tableName, pageCounter);
    if (!isPermanent())
        bufferManager.deleteFile(this->sourceFileName);
}

/**
 * @brief Function that returns a cursor that reads rows from this table
 *
 * @return Cursor
 */
Cursor Table::getCursor() {
    logger.log("Table::getCursor");
    Cursor cursor(this->tableName, 0);
    return cursor;
}
/**
 * @brief Function that returns the index of column indicated by columnName
 *
 * @param columnName
 * @return int
 */
int Table::getColumnIndex(string columnName) {
    logger.log("Table::getColumnIndex");
    for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++) {
        if (this->columns[columnCounter] == columnName)
            return columnCounter;
    }
}
/**
 * @brief Function that deletes the table object along with its associated data file.
 *
 * from the file system. Use this function when you want to permanently delete a table and its data.
 */
void Table::deleteTable() {
    logger.log("Table::deleteTable - Start");

    // Delete the associated file
    string filePath = this->sourceFileName;
    if (remove(filePath.c_str()) == 0) {
        logger.log("Deleted file: " + filePath);
    } else {
        logger.log("Failed to delete file: " + filePath);
    }

    // Delete the table object itself
    delete this;

    logger.log("Table::deleteTable - End");
}

static bool sortComparator(const vector<int> &a, const vector<int> &b) {
    logger.log("Inside Sort Comp");
    for (int i = 0; i < columnIndexes.size(); i++) {
        logger.log("SORT COL INDEX: " + to_string(columnIndexes[i]));
        if (a[columnIndexes[i]] != b[columnIndexes[i]]) {
            if (sortValues[i] == 0)
                return a[columnIndexes[i]] < b[columnIndexes[i]];
            else if (sortValues[i] == 1)
                return a[columnIndexes[i]] > b[columnIndexes[i]];
        }
    }
    return false;
}

struct MyPair {
    vector<int> row;
    int blockNum;
    int boundary;
    int cursorIndex;
};

struct CompareByFirstElement {
    bool operator()(const MyPair &lhs, const MyPair &rhs) const {
        for (int i = 0; i < columnIndexes.size(); i++) {
            if (lhs.row[columnIndexes[i]] != rhs.row[columnIndexes[i]]) {
                if (sortValues[i] == 0)
                    return lhs.row[columnIndexes[i]] > rhs.row[columnIndexes[i]];
                else if (sortValues[i] == 1)
                    return lhs.row[columnIndexes[i]] < rhs.row[columnIndexes[i]];
            }
        }
        return false;
    }
};

// K-way merging step
void Table::externalSort() {
    int mergeIterations = ceil(log((int)this->blockCount) / log((int)BLOCK_COUNT - 1)) - 1;
    // cout << "Total Iterations: " << mergeIterations << endl;
    vector<Cursor> pageCursor;
    // K-way merge
    int K = BLOCK_COUNT - 1;
    for (int i = 0; i <= mergeIterations; i++) {
        logger.log("In Iteration: " + to_string(i + 1));

        int size = pow(K, i);
        logger.log("Size: " + to_string(size));
        int clusters = ceil(((double)this->blockCount) / (double)(size));
        vector<pair<int, int>> clusterData(clusters);
        logger.log("Total Clusters: " + to_string(clusters));
        logger.log("TOTAL Table BLOCKS: " + to_string(this->blockCount));
        int s = 0, e = size - 1;
        for (int i = 0; i < clusters; i++) {
            clusterData[i] = {s, e};
            s += size;
            e += size;
            e = min(e, (int)this->blockCount - 1);
        }

        int globalPageIndex = 0;
        // vector<vector<int>> mergeData;
        logger.log("Hello-->>" + to_string(clusters) + " " + to_string(K) + " " + to_string(ceil((double)clusters / (double)K)));
        for (int i = 0; i < ceil((double)clusters / (double)K); i++) {
            int start = i * size * K;
            vector<pair<int, int>> boundary;
            logger.log("Cluster num: " + to_string(i));
            while (start < min((int)this->blockCount, K * size * (i + 1))) {
                boundary.push_back({start, min(start + size - 1, (int)this->blockCount - 1)});
                logger.log("Values: " + to_string(start) + "*****" + to_string(min(start + size - 1, (int)this->blockCount - 1)));
                start += size;
            }
            vector<vector<int>> mergeData;
            for (int j = 0; j < boundary.size(); j += K) {
                int bstart = j, bend = min(j + K - 1, (int)boundary.size() - 1);
                logger.log("Boundary taken: " + to_string(bstart) + " " + to_string(bend));
                logger.log("Making Cursor Vectors");
                vector<pair<Cursor, int>> cursorVec;
                for (int r = bstart; r <= bend; r++) {
                    Cursor cursor(this->tableName, boundary[r].first);
                    logger.log(to_string(r) + "------" + to_string(boundary[r].first));
                    cursorVec.push_back({cursor, boundary[r].first});
                }
                logger.log("Made Cursor Vectors");

                priority_queue<MyPair, vector<MyPair>, CompareByFirstElement> pq;
                logger.log("Putting data in Priority Queue for iteration: " + to_string(j));
                for (int l = 0; l < cursorVec.size(); l++) {
                    vector<int> data = cursorVec[l].first.getNextPageRow();
                    pq.push({data, cursorVec[l].first.pageIndex, cursorVec[l].second, l});
                }
                logger.log("DATA PUT!!");
                logger.log("TOTAL Table BLOCKS: " + to_string(this->blockCount));

                while (!pq.empty()) {
                    logger.log("Inside While");
                    MyPair element = pq.top();
                    pq.pop();
                    mergeData.push_back(element.row);

                    int pageNum = element.blockNum;
                    int boundaryVal = element.boundary;
                    int cursorIndex = element.cursorIndex;
                    logger.log("PageIndex,Boundary,CursorIndex,PagePointer: " + to_string(pageNum) + " " + to_string(boundaryVal) + " " + to_string(cursorIndex) + " " + to_string(cursorVec[cursorIndex].first.pagePointer));
                    vector<int> newRow = cursorVec[cursorIndex].first.getNextPageRow();
                    string rowval = "";
                    for (int i = 0; i < newRow.size(); i++) {
                        rowval += to_string(newRow[i]) + " ";
                    }
                    logger.log("Row data: " + rowval);
                    logger.log("Page Num: " + to_string(pageNum));
                    logger.log("New Row size:" + to_string(newRow.size()));
                    if (newRow.size() != 0) {
                        logger.log("HERE 1");
                        // MyPair elementDum = pq.top();
                        // logger.log(to_string(elementDum.blockNum));
                        pq.push({newRow, cursorVec[cursorIndex].first.pageIndex, boundaryVal, cursorIndex});
                        MyPair elementDum = pq.top();
                        logger.log(to_string(elementDum.blockNum));
                    } else {
                        logger.log("HERE 2 " + to_string(boundary[cursorIndex].second));
                        if (cursorVec[cursorIndex].first.pageIndex < boundary[cursorIndex].second) {
                            vector<int> newData = cursorVec[cursorIndex].first.getNext();
                            string rowval = "";
                            for (int i = 0; i < newData.size(); i++) {
                                rowval += to_string(newData[i]) + " ";
                            }
                            logger.log("Row data: " + rowval);
                            pq.push({newData, cursorVec[cursorIndex].first.pageIndex, boundaryVal, cursorIndex});
                        } else
                            continue;
                    }
                    if (mergeData.size() == (int)this->maxRowsPerBlock) {
                        logger.log("MERGE DATA SIZE: " + to_string((int)mergeData.size()));
                        logger.log("HERE 3");
                        for (int y = 0; y < mergeData.size(); y++) {
                            string tmp = "";
                            for (int z = 0; z < mergeData[y].size(); z++) {
                                tmp += to_string(mergeData[y][z]) + " ";
                            }
                            logger.log(tmp);
                        }
                        logger.log("=============PRINTED===============");
                        // bufferManager.writePage(this->tableName, globalPageIndex, mergeData, (int)this->maxRowsPerBlock);
                        // this->pageName = "../data/temp/" + this->tableName + "_Page" + to_string(pageIndex);
                        logger.log("Creating-->");
                        logger.log("../data/temp/" + this->tableName + "Dummy" + to_string(globalPageIndex));
                        ofstream fout("../data/temp/" + this->tableName + "Dummy" + to_string(globalPageIndex));
                        for (int rowCounter = 0; rowCounter < mergeData.size(); rowCounter++) {
                            for (int columnCounter = 0; columnCounter < mergeData[rowCounter].size(); columnCounter++) {
                                if (columnCounter != 0)
                                    fout << " ";
                                fout << mergeData[rowCounter][columnCounter];
                            }
                            fout << endl;
                        }
                        fout.close();

                        mergeData.clear();
                        globalPageIndex++;
                    }
                    logger.log("Last Line of While");
                }
                if (mergeData.size() > 0) {
                    logger.log("MERGE DATA SIZE: " + to_string((int)mergeData.size()));
                    // bufferManager.writePage(this->tableName, globalPageIndex, mergeData, (int)mergeData.size());
                    for (int y = 0; y < mergeData.size(); y++) {
                        string tmp = "";
                        for (int z = 0; z < mergeData[y].size(); z++) {
                            tmp += to_string(mergeData[y][z]) + " ";
                        }
                        logger.log(tmp);
                    }
                    logger.log("=============PRINTED===============OUTSIDE");

                    logger.log("Creating-->");
                    logger.log("../data/temp/" + this->tableName + "Dummy" + to_string(globalPageIndex));
                    ofstream fout("../data/temp/" + this->tableName + "Dummy" + to_string(globalPageIndex));
                    for (int rowCounter = 0; rowCounter < mergeData.size(); rowCounter++) {
                        for (int columnCounter = 0; columnCounter < mergeData[rowCounter].size(); columnCounter++) {
                            if (columnCounter != 0)
                                fout << " ";
                            fout << mergeData[rowCounter][columnCounter];
                        }
                        fout << endl;
                    }
                    fout.close();

                    mergeData.clear();
                    globalPageIndex++;
                }
            }
        }
        // logger.log("Here gpi: " + to_string(globalPageIndex));
        for (int u = 0; u < globalPageIndex; u++) {
            logger.log("Merging Page:" + to_string(u));
            ifstream file("../data/temp/" + this->tableName + "Dummy" + to_string(u));
            if (!file) {
                cout << "../data/temp/" + this->tableName + "Dummy" + to_string(u) << endl;
                cerr << "Failed to open the file." << endl;
            }

            vector<vector<int>> matrix;
            string line;

            while (getline(file, line)) {
                vector<int> row;
                istringstream iss(line);
                int value;

                while (iss >> value) {
                    row.push_back(value);
                }

                matrix.push_back(row);
            }
            bufferManager.writePage(this->tableName, u, matrix, (int)matrix.size());

            string fname = "../data/temp/" + this->tableName + "Dummy" + to_string(u);

            if (remove(fname.c_str()) == 0) {
                logger.log("Temp File deleted successfully.");
            } else {
                logger.log("Error deleting Temp File");
            }
        }
    }
    sortValues.clear();
    columnIndexes.clear();
    parsedQuery.sortColumns.clear();
    parsedQuery.sortStrategy.clear();
}

void Table::sortTable() {
    logger.log("Inside Table class sort function");
    // 0-> asc, 1-> desc
    sortValues.resize(parsedQuery.sortStrategy.size(), 0);

    columnIndexes.resize(parsedQuery.sortStrategy.size());
    logger.log("Checking....");
    for (int i = 0; i < parsedQuery.sortStrategy.size(); i++)
        if (parsedQuery.sortStrategy[i] == DESC)
            sortValues[i] = 1;
    logger.log("Sort Vals Set....");
    // Table *rlnTable = tableCatalogue.getTable(parsedQuery.sortRelationName);
    logger.log("Get Column Indexed to be Sorted...");
    for (int i = 0; i < parsedQuery.sortColumns.size(); i++){
        // cout<<parsedQuery.sortColumns[i]<<endl;
        columnIndexes[i] = this->getColumnIndex(parsedQuery.sortColumns[i]);
    }
    logger.log("Completed Column Indexed to be Sorted...");
    logger.log("SortVals and ColumnIndex size: " + to_string((int)sortValues.size()) + " " + to_string((int)columnIndexes.size()));
    // Table *tempTable = new Table("temp_" + parsedQuery.sortRelationName, rlnTable->columns);
    long long pageDataRows = (this->maxRowsPerBlock), cnt = 0;
    vector<vector<int>> pagesData;
    Cursor cursor = this->getCursor();
    int pageIndex = 0;

    while (1) {
        vector<int> dataRow = cursor.getNext();
        if (dataRow.size() == 0)
            break;
        pagesData.push_back(dataRow);
        cnt++;

        if (cnt == pageDataRows) {
            logger.log("Writing Page back in Sorted Way!!!");
            sort(pagesData.begin(), pagesData.end(), sortComparator);
            logger.log("Page Data Sorted!!");
            for (int r = 0; r < pagesData.size(); r++)
                bufferManager.writePage(this->tableName, pageIndex, pagesData, cnt);
            // tempTable->writeRow(pagesData[r]);
            logger.log("Collect Data Again!!");
            cnt = 0;
            pageIndex++;
            pagesData.clear();
        }
    }
    logger.log("Checking Overflow Data");
    if (pagesData.size() > 0) {
        sort(pagesData.begin(), pagesData.end(), sortComparator);
        for (int r = 0; r < pagesData.size(); r++)
            bufferManager.writePage(this->tableName, pageIndex, pagesData, cnt);
        // tempTable->writeRow(pagesData[r]);
        cnt = 0;
        pagesData.clear();
    }
    logger.log("Calling External Sort");
    this->externalSort();
    this->makePermanent();
}

void Table::joinTables() {
    logger.log("Table::joinTables - Start");
    string newRelationName = parsedQuery.joinResultRelationName;
    string tableName1 = parsedQuery.joinFirstRelationName;
    string tableName2 = parsedQuery.joinSecondRelationName;
    string column1 = parsedQuery.joinFirstColumnName;
    BinaryOperator binOp = parsedQuery.joinBinaryOperator;
    string column2 = parsedQuery.joinSecondColumnName;


    Table *table1 = tableCatalogue.getTable(tableName1);
    Table *table2 = tableCatalogue.getTable(tableName2);


    string tempTableName1 = "temp_GroupBy1";
    string tempTableName2 = "temp_GroupBy2";
    Table *sortedTable1 = new Table(tempTableName1, table1->columns);  
    Table *sortedTable2 = new Table(tempTableName2, table2->columns);  

    Cursor cursor1 = table1->getCursor();
    vector<int> row1 = cursor1.getNext();
    logger.log("Copying rows to the temp_groupBy table");
    while (!row1.empty()) {
        sortedTable1->writeRow(row1);
        row1 = cursor1.getNext();
    }
    logger.log("Copying rows to the temp_GroupBy table (1)");
    Cursor cursor2 = table2->getCursor();
    vector<int> row2 = cursor2.getNext();
    logger.log("Copying rows to the temp_GroupBy table (2)");
    while (!row2.empty()) {
        sortedTable2->writeRow(row2);
        row2 = cursor2.getNext();
    }
    parsedQuery.loadRelationName = tempTableName1;
    executeLOAD();
    sortedTable1 = tableCatalogue.getTable(tempTableName1);

    parsedQuery.loadRelationName = tempTableName2;
    executeLOAD();
    sortedTable2 = tableCatalogue.getTable(tempTableName2);

    logger.log("External sorting: for both tables");
    parsedQuery.sortStrategy.clear();
    parsedQuery.sortStrategy.push_back(ASC);
    parsedQuery.sortColumns.clear();
    parsedQuery.sortColumns.push_back(column2);

    sortedTable2->sortTable();
    parsedQuery.sortStrategy.clear();
    parsedQuery.sortStrategy.push_back(ASC);
    parsedQuery.sortColumns.clear();
    parsedQuery.sortColumns.push_back(column1);

    sortedTable1->sortTable();

    logger.log("Perform the JOIN operation");

    vector<string> header = table1->columns;
    vector<string> header2 = table2->columns;

    header.insert(header.end(), header2.begin(), header2.end());
    Table *resultTable = new Table(newRelationName, header);

    cursor1 = sortedTable1->getCursor();
    cursor2 = sortedTable2->getCursor();

    int index1 = sortedTable1->getColumnIndex(column1);
    int index2 = sortedTable2->getColumnIndex(column2);

    row1 = cursor1.getNext();
    row2 = cursor2.getNext();

    logger.log("Preparing : Page Pointers");
    int currRow = 0;
    long long totalRow = 0;
    int pageCounter = 0;
    vector<vector<int>> pageData;
    logger.log("Comparing and combining rows from both tables");

    bool (*conditionFunction)(int, int) = nullptr;

    switch (binOp) {
        case EQUAL:
            conditionFunction = [](int val1, int val2) { return val1 == val2; };
            break;
        case GREATER_THAN:
            conditionFunction = [](int val1, int val2) { return val1 > val2; };
            break;
        case GEQ:
            conditionFunction = [](int val1, int val2) { return val1 >= val2; };
            break;
        case LESS_THAN:
            conditionFunction = [](int val1, int val2) { return val1 < val2; };
            break;
        case LEQ:
            conditionFunction = [](int val1, int val2) { return val1 <= val2; };
            break;
    }

    while (!row1.empty() && !row2.empty()) {
        int num1 = row1[index1];
        int num2 = row2[index2];
        logger.log("Inside the while loop - Before condition check");

        if (conditionFunction(num1, num2)) {
            vector<int> newRow = row1;
            newRow.insert(newRow.end(), row2.begin(), row2.end());
            pageData.push_back(newRow);
            currRow++;
            totalRow++;
            // Move to the next row
            if (currRow == resultTable->maxRowsPerBlock) {
                bufferManager.writePage(resultTable->tableName, pageCounter, pageData, currRow);
                pageCounter++;
                currRow = 0;
                pageData.clear();
            }
            logger.log("Inside the while loop - After writing newRow");

            Cursor cursor1i = cursor1;
            vector<int> row1i = cursor1i.getNext();
            while (row1i.size() && conditionFunction(row1i[index1], row2[index2])) {
                newRow = row1i;
                newRow.insert(newRow.end(), row2.begin(), row2.end());
                pageData.push_back(newRow);
                currRow++;
                totalRow++;

                // Move to the next row
                if (currRow == resultTable->maxRowsPerBlock) {
                    bufferManager.writePage(resultTable->tableName, pageCounter, pageData, currRow);
                    pageCounter++;
                    currRow = 0;
                    pageData.clear();
                }
                logger.log("Inside the while loop - After writing newRow in inner loop");

                row1i = cursor1i.getNext();
            }

            Cursor cursor2i = cursor2;
            vector<int> row2i = cursor2i.getNext();
            while (row2i.size() && conditionFunction(row1[index1], row2i[index2])) {
                newRow = row1;
                newRow.insert(newRow.end(), row2i.begin(), row2i.end());
                pageData.push_back(newRow);
                currRow++;
                totalRow++;

                // Move to the next row
                if (currRow == resultTable->maxRowsPerBlock) {
                    bufferManager.writePage(resultTable->tableName, pageCounter, pageData, currRow);
                    pageCounter++;
                    currRow = 0;
                    pageData.clear();
                }
                logger.log("Inside the while loop - After writing newRow from cursor2i");

                row2i = cursor2i.getNext();
            }
            row1 = cursor1.getNext();
            row2 = cursor2.getNext();

        } else {
            if (binOp == EQUAL) {
                if (row1[index1] > row2[index2]) {
                    row2 = cursor2.getNext();
                } else {
                    row1 = cursor1.getNext();
                }
            } else if (binOp == GREATER_THAN) {
                row1 = cursor1.getNext();
            } else if (binOp == GEQ) {
                row1 = cursor1.getNext();
            } else if (binOp == LESS_THAN) {
                row2 = cursor2.getNext();
            } else if (binOp == LEQ) {
                row2 = cursor2.getNext();
            }
        }
    }
    if (pageData.size() > 0) {
        bufferManager.writePage(resultTable->tableName, pageCounter, pageData, currRow);
        pageCounter++;
    }
    resultTable->rowCount = totalRow;
    resultTable->columnCount = header.size();
    logger.log("Pages created :: Total NumRows : " + to_string(resultTable->rowCount) + ", Total Pages : " + to_string(pageCounter) + ", ColCount : " + to_string(resultTable->columnCount));
    logger.log("LOG :: Before make permanent");
    tableCatalogue.insertTable(resultTable);
    ofstream outputFile("../data/" + resultTable->tableName + ".csv", ios::app);
    for (int i = 0; i < pageCounter; i++) {
        ifstream inputFile("../data/temp/" + resultTable->tableName + "_Page" + to_string(i));
        string line;
        while (getline(inputFile, line)) {
            istringstream iss(line);
            string word;
            bool firstWord = true;
            while (iss >> word) {
                if (!firstWord) {
                    outputFile << ", ";  
                }
                outputFile << word;
                firstWord = false;
            }
            outputFile << endl;
        }
        inputFile.close();
    }
    outputFile.close();
    logger.log("After make permanent");
    sortedTable1->unload();
    sortedTable2->unload();
    sortedTable1->deleteTable();
    sortedTable2->deleteTable();
    logger.log("Table::joinTables - End");
}

void Table::orderBy() {
    logger.log("Table::orderBy - Start");
    string newTableName = parsedQuery.orderResultRelation;
    Table *sortedTable = new Table(newTableName, this->columns);  

    Cursor cursor = this->getCursor();
    vector<int> row = cursor.getNext();
    while (!row.empty()) {
        sortedTable->writeRow(row);
        row = cursor.getNext();
    }

    parsedQuery.loadRelationName = newTableName;
    executeLOAD();
    sortedTable = tableCatalogue.getTable(newTableName);


    parsedQuery.sortStrategy.clear();
    parsedQuery.sortStrategy.push_back(parsedQuery.sortingStrategy);
    parsedQuery.sortColumns.clear();
    parsedQuery.sortColumns.push_back(parsedQuery.orderAttribute);
    sortedTable->sortTable();
    logger.log("Table :: OrderBy : End");
}

void Table::groupBy() {
    logger.log("Table::groupBy");
    string newTableName = parsedQuery.groupResultRelation;
    string groupingAttribute = parsedQuery.groupAttribute;
    string havingaggregateAttribute = parsedQuery.havingAttribute;
    Aggregate havingaggregateFunction = parsedQuery.havingAgg;
    BinaryOperator binOp = parsedQuery.groupBinaryOperator;
    int attributeValue = parsedQuery.havingValue;
    string aggregateAttribute = parsedQuery.returnAttribute;
    Aggregate aggregateFunction = parsedQuery.returnAgg;
    string stringaggregateFunction = parsedQuery.returnAggregate;

    logger.log("External sorting for GROUP BY");
    parsedQuery.sortStrategy.clear();
    parsedQuery.sortStrategy.push_back(ASC);
    parsedQuery.sortColumns.clear();
    parsedQuery.sortColumns.push_back(parsedQuery.groupAttribute);
    string tempTableName = "temp_GroupBy";
    Table *sortedTable = new Table(tempTableName, this->columns);  

    Cursor cursor = this->getCursor();
    vector<int> row = cursor.getNext();
    logger.log("Copying rows to the temp_groupBy table");
    while (!row.empty()) {
        sortedTable->writeRow(row);
        row = cursor.getNext();
    }
    logger.log("after copying");
    parsedQuery.loadRelationName = tempTableName;
    logger.log(to_string(parsedQuery.sortColumns.size()));
    executeLOAD();
    sortedTable = tableCatalogue.getTable(tempTableName);
    sortedTable->sortTable();
    logger.log("before sorting");

    vector<string> header = {groupingAttribute, stringaggregateFunction + "(" + aggregateAttribute + ")"};
    Table *groupedTable = new Table(newTableName, header);
    logger.log("Create a new table for the result");
    
    vector<int> currentGroup;
    int currentGroupValue = -1;
    logger.log("Initialize variables for grouping");
    
    int havingaggregateResult = 0;
    int havingaggregateCount = 0;
    int aggregationResult = 0;
    int aggregateCount = 0;

    // Get column indices for groupingAttribute and aggregateAttribute
    int groupIndex = sortedTable->getColumnIndex(groupingAttribute);
    int havingaggregateIndex = sortedTable->getColumnIndex(havingaggregateAttribute);
    int aggregateIndex = sortedTable->getColumnIndex(aggregateAttribute);
    logger.log("Get column indices for groupingAttribute and aggregateAttribute");
    
    cursor = sortedTable->getCursor();
    row = cursor.getNext();
    logger.log("Step 7");
    logger.log("Performing GROUP BY and Aggregation");
    int currRow = 0;
    long long totalRow = 0;
    int pageCounter = 0;
    // groupedTable->sourceFileName = "../data/temp/"+groupedTable->tableName+"_Page"+to_string(pageCounter);
    vector<vector<int>> pageData;
    while (true) {
        if (row.empty()) {
            
            if (havingaggregateFunction == AVG && havingaggregateCount>0)
                havingaggregateResult /= havingaggregateCount;
            if (currentGroupValue != -1 && havingaggregateCount>0 && ((binOp == EQUAL && attributeValue == havingaggregateResult) ||
                                            (binOp == GREATER_THAN && attributeValue < havingaggregateResult) ||
                                            (binOp == GEQ && attributeValue <= havingaggregateResult) ||
                                            (binOp == LESS_THAN && attributeValue > havingaggregateResult) ||
                                            (binOp == LEQ && attributeValue >= havingaggregateResult))) {
                // Use the aggregateFunction to decide what to add
                if (aggregateFunction == MIN || aggregateFunction == MAX || aggregateFunction == SUM) {
                    // groupedTable->writeRow(vector<int>
                    pageData.push_back({currentGroupValue, aggregationResult});

                    currRow++;
                    totalRow++;
                } else if (aggregateFunction == COUNT) {
                    // groupedTable->writeRow(vector<int>{currentGroupValue, aggregateCount});
                    pageData.push_back({currentGroupValue, aggregateCount});
                    currRow++;
                    totalRow++;
                } else if (aggregateFunction == AVG) {
                    // groupedTable->writeRow(vector<int>{currentGroupValue, aggregationResult / aggregateCount});
                    pageData.push_back({currentGroupValue, aggregationResult / aggregateCount});
                    currRow++;
                    totalRow++;
                }
            }
            break;
        }
        int groupValue = row[groupIndex];
        int aggregateValue = row[aggregateIndex];
        int havingaggregateValue = row[havingaggregateIndex];

        if (groupValue != currentGroupValue) {
            // Start a new group
            if (havingaggregateFunction == AVG && havingaggregateCount>0)
                havingaggregateResult /= havingaggregateCount;
            if (currentGroupValue != -1 && havingaggregateCount>0 && ((binOp == EQUAL && attributeValue == havingaggregateResult) ||
                                            (binOp == GREATER_THAN && attributeValue < havingaggregateResult) ||
                                            (binOp == GEQ && attributeValue <= havingaggregateResult) ||
                                            (binOp == LESS_THAN && attributeValue > havingaggregateResult) ||
                                            (binOp == LEQ && attributeValue >= havingaggregateResult))) {
                                                
                if (aggregateFunction == MIN || aggregateFunction == MAX || aggregateFunction == SUM) {
                    // groupedTable->writeRow(vector<int>
                    pageData.push_back({currentGroupValue, aggregationResult});

                    currRow++;
                    totalRow++;
                } else if (aggregateFunction == COUNT) {
                    // groupedTable->writeRow(vector<int>{currentGroupValue, aggregateCount});
                    pageData.push_back({currentGroupValue, aggregateCount});
                    currRow++;
                    totalRow++;
                } else if (aggregateFunction == AVG) {
                    // groupedTable->writeRow(vector<int>{currentGroupValue, aggregationResult / aggregateCount});
                    pageData.push_back({currentGroupValue, aggregationResult / aggregateCount});
                    currRow++;
                    totalRow++;
                }
            }
            
            currentGroupValue = groupValue;
            currentGroup.clear();
            aggregationResult = 0;
            aggregateCount = 0;
            havingaggregateCount = 0;
            havingaggregateResult = 0;
        }

        // Perform aggregation based on the aggregateFunction
        if (aggregateFunction == MIN) {
            aggregationResult = min(aggregationResult, aggregateValue);
        } else if (aggregateFunction == MAX) {
            aggregationResult = max(aggregationResult, aggregateValue);
        } else if (aggregateFunction == SUM) {
            aggregationResult += aggregateValue;
        } else if (aggregateFunction == COUNT) {
            aggregateCount++;
        } else if (aggregateFunction == AVG) {
            aggregationResult += aggregateValue;
            aggregateCount++;
        }

        if (havingaggregateFunction == MIN) {
            havingaggregateResult = min(aggregationResult, havingaggregateValue);
        } else if (havingaggregateFunction == MAX) {
            havingaggregateResult = max(aggregationResult, havingaggregateValue);
        } else if (havingaggregateFunction == SUM) {
            havingaggregateResult += havingaggregateValue;
        } else if (havingaggregateFunction == COUNT) {
            havingaggregateCount++;
            havingaggregateResult++;
        } else if (havingaggregateFunction == AVG) {
            havingaggregateResult += havingaggregateValue;
            havingaggregateCount++;
        }

        // Move to the next row
        if (currRow == groupedTable->maxRowsPerBlock) {
            bufferManager.writePage(groupedTable->tableName, pageCounter, pageData, currRow);
            pageCounter++;
            currRow = 0;
            pageData.clear();
            // groupedTable->sourceFileName = "../data/temp/"+groupedTable->tableName+"_Page"+to_string(pageCounter);
        }
        row = cursor.getNext();
    }
    if (pageData.size() > 0) {
        bufferManager.writePage(groupedTable->tableName, pageCounter, pageData, currRow);
        pageCounter++;
    }
    logger.log("pages created");
    sortedTable->unload();
    // groupedTable->sourceFileName = "../data/"+groupedTable->tableName+".csv";
    groupedTable->rowCount = totalRow;
    groupedTable->columnCount = 2;
    logger.log("before make permanent");
    tableCatalogue.insertTable(groupedTable);
    ofstream outputFile("../data/" + groupedTable->tableName + ".csv", ios::app);
    // outputFile<<header<<endl;
    for (int i = 0; i < pageCounter; i++) {
        ifstream inputFile("../data/temp/" + groupedTable->tableName + "_Page" + to_string(i));
        string line;
        while (getline(inputFile, line)) {
            istringstream iss(line);
            string word;
            bool firstWord = true;
            while (iss >> word) {
                if (!firstWord) {
                    outputFile << ", ";  
                }
                outputFile << word;
                firstWord = false;
            }
            outputFile << endl;
        }
        inputFile.close();
    }
    outputFile.close();
    logger.log("After make permanent");
    sortedTable->unload();
    sortedTable->deleteTable();
    logger.log("Table::GroupBy - End");
}