#include "global.h"
/**
 * @brief Construct a new Matrix:: Matrix object used in the case where the data
 * file is available and LOAD command has been called. This command should be
 * followed by calling the LOAD MATRIX function;
 *
 * @param matrixName
 */
Matrix::Matrix(string queryMatrixName)
{
    logger.log("Matrix::Matrix");
    this->matrixSourceFileName = "../data/" + queryMatrixName + ".csv";
    this->matrixName = queryMatrixName;
}

/**
 * @brief The load function is used when the LOAD MATRIX command is encountered. It
 * reads data from the source file, splits it into blocks/pages and updates matrix
 * statistics.
 *
 * @return true if the matrix has been successfully loaded
 * @return false if an error occurred
 */
bool Matrix::load()
{
    logger.log("Matrix::load (in matrix.cpp)");
    fstream fin(this->matrixSourceFileName, ios::in);

    if (!fin.is_open())
    {
        cout << "Could not open the matrix file." << endl;
        logger.log("Matrix::load, cannot open file(in matrix.cpp)");
        return false;
    }

    // streampos fileSize = fin.tellg();
    // if (fileSize == 0)
    // {
    //     cout << "Matrix File is empty." << endl;
    //     logger.log("Matrix::load, file is empty(in matrix.cpp)");
    //     return false;
    // }
    // set Matrix object variables and then call blockify()
    string firstRow = "";
    if (getline(fin, firstRow))
    {
        this->setDimensions(firstRow);
        return this->matrixBlockify();
    }
    else
    {
        cout << "Error in opening Matrix File." << endl;
        logger.log("Matrix::load, error opening file(in matrix.cpp)");
        return false;
    }
    fin.close();
    return false;
}

void Matrix::setDimensions(string firstRow)
{
    logger.log("Matrix::setDimensions (in matrix.cpp)");
    istringstream iss(firstRow);
    string word;
    int colsCount = 0;

    while (getline(iss, word, ','))
    {
        // Ignore leading and trailing spaces if any
        size_t start = word.find_first_not_of(" \t");
        size_t end = word.find_last_not_of(" \t");

        if (start != string::npos)
        {
            colsCount++;
        }
    }

    cout << "Total number of cols/rows(in matrix.cpp): " << colsCount << endl;
    this->colsDim = this->rowsDim = colsCount;
    this->pages = ceil(((double)this->colsDim) / ((double)matrixDims));
}

vector<int> Matrix::insertRowInPage(string rowData, int colNum)
{
    logger.log("Matrix::insertRowInPage(in matrix.cpp)");
    istringstream iss(rowData);
    string data;
    int i = 0;
    // skip previous column elements
    for (; i < matrixDims * colNum; i++)
    {
        if (!getline(iss, data, ','))
        {
            logger.log("Matrix::insertRowInPage, Parsing error!!(in matrix.cpp)");
            return vector<int>(-999);
        }
    }

    vector<int> rowVals;
    for (; i < min(matrixDims * (colNum + 1), this->colsDim); i++)
    {
        if (getline(iss, data, ','))
        {
            size_t start = data.find_first_not_of(" \t");
            size_t end = data.find_last_not_of(" \t");
            if (start != string::npos)
            {
                string trimmedvalue = data.substr(start, end - start + 1);
                rowVals.push_back(stoi(trimmedvalue));
            }
            else
            {
                logger.log("Matrix::insertRowInPage, Parsing error!!(in matrix.cpp)");
                return vector<int>(-999);
            }
        }
        else
        {
            logger.log("Matrix::insertRowInPage, Parsing error!!(in matrix.cpp)");
            return vector<int>(-999);
        }
    }

    while ((int)rowVals.size() < matrixDims)
        rowVals.push_back(0);
    return rowVals;
}

bool Matrix::matrixBlockify()
{
    int blocksWritten = 0, blocksRead = 0;
    logger.log("Matrix::matrixBlockify(in matrix.cpp)");

    // start storing data in pages/blocks in colsDim wise manner
    for (int col = 0; col < this->pages; col++)
    {
        ifstream fin(this->matrixSourceFileName, ios::in);
        vector<vector<int>> pageData(matrixDims, vector<int>(matrixDims, 0));
        string rowData;
        int row = 0, pageRow = 0;
        // int pageCol = 0;
        // if(col == this->pages - 1) pageCol = matrixDims - (this->colsDim % matrixDims);
        while (getline(fin, rowData))
        {
            if (pageRow == matrixDims)
            {
                // cout << "Writing Page having r and c:" << row << " " << col << endl;
                bufferManager.writeMatrix_Page(this->matrixName, row, col, pageData);
                blocksWritten++;
                // clear the vector
                // this_page.assign(MATRIX_PAGE_DIM, vector<int> (MATRIX_PAGE_DIM, -1));
                // pageData.resize(matrixDims);
                pageData.assign(matrixDims, vector<int>(matrixDims, 0));
                pageRow = 0;
                row++;
            }

            pageData[pageRow] = this->insertRowInPage(rowData, col);
            pageRow++;
        }
        // cout << "Writing Page having r and c:" << row << " " << col << endl;
        // cout << "Data for row " << pageRow << endl;
        // for(int ele:pageData[pageRow]) cout << ele << " ";
        // cout << endl;
        // pageRow, pageCols
        blocksWritten++;
        bufferManager.writeMatrix_Page(this->matrixName, row, col, pageData);
        fin.close();
    }
    cout << "Number of blocks written: " << blocksWritten << endl;
    cout << "Number of blocks accessed: " << blocksWritten << endl;
    return true;
}

/**
 * @brief Function to compute Transpose(Matrix) operation
 *
 */
void Matrix::doTranspose()
{
    int blocksWritten = 0, blocksRead = 0;
    for (int currRow = 0; currRow < this->pages; currRow++)
    {
        for (int currCol = currRow; currCol < this->pages; currCol++)
        {
            // diagonal page blocks.....
            if (currRow == currCol)
            {
                blocksRead++;
                Page currPage = bufferManager.getMatrix(this->matrixName, currRow, currCol);
                vector<vector<int>> currPageEle = currPage.getMatrixPageEle();
                for (int i = 0; i < matrixDims; i++)
                {
                    for (int j = i + 1; j < matrixDims; j++)
                    {
                        int temp = currPageEle[i][j];
                        currPageEle[i][j] = currPageEle[j][i];
                        currPageEle[j][i] = temp;
                    }
                }
                // if(currRow==0 && currCol==0){
                //     for(int i=0;i<matrixDims;i++){
                //     for(int j=i+1;j<matrixDims;j++){
                //         cout<<currPageEle[i][j]<<" ";
                //     }
                //     cout<<endl;
                // }
                // }
                bufferManager.writeMatrix_Page(this->matrixName, currRow, currCol, currPageEle);
                blocksWritten++;
                bufferManager.clearMainMemory();
            }
            else
            {
                Page page1 = bufferManager.getMatrix(this->matrixName, currRow, currCol);
                Page page2 = bufferManager.getMatrix(this->matrixName, currCol, currRow);
                blocksRead += 2;
                vector<vector<int>> matrixPage1 = page1.getMatrixPageEle();
                vector<vector<int>> matrixPage2 = page2.getMatrixPageEle();

                for (int i = 0; i < matrixDims; i++)
                {
                    for (int j = 0; j < matrixDims; j++)
                    {
                        swap(matrixPage1[i][j], matrixPage2[j][i]);
                    }
                }

                bufferManager.writeMatrix_Page(this->matrixName, currRow, currCol, matrixPage1);
                bufferManager.writeMatrix_Page(this->matrixName, currCol, currRow, matrixPage2);
                blocksWritten += 2;
                bufferManager.clearMainMemory();
            }
        }
    }
    cout << "Number of blocks written:   " << blocksWritten << endl;
    cout << "Number of blocks read:  " << blocksRead << endl;
    cout << "Number of blocks accessed:   " << blocksWritten + blocksRead << endl;
}

void Matrix::doCheckSymmetry()
{
    int blocksWritten = 0, blocksRead = 0;

    for (int currRow = 0; currRow < this->pages; currRow++)
    {
        for (int currCol = currRow; currCol < this->pages; currCol++)
        {
            // diagonal block/ pages.....
            if (currRow == currCol)
            {
                Page currPage = bufferManager.getMatrix(this->matrixName, currRow, currCol);
                blocksRead++;
                vector<vector<int>> currPageEle = currPage.getMatrixPageEle();
                for (int i = 0; i < matrixDims; i++)
                {
                    for (int j = i + 1; j < matrixDims; j++)
                    {
                        if (currPageEle[i][j] != currPageEle[j][i])
                        {
                            cout << "FALSE" << endl;
                            return;
                        }
                    }
                }
            }

            // non-diagonal two blockas ata a time.....
            else
            {
                Page page1 = bufferManager.getMatrix(this->matrixName, currRow, currCol);
                Page page2 = bufferManager.getMatrix(this->matrixName, currCol, currRow);
                blocksRead += 2;
                vector<vector<int>> matrixPage1 = page1.getMatrixPageEle();
                vector<vector<int>> matrixPage2 = page2.getMatrixPageEle();

                for (int i = 0; i < matrixDims; i++)
                {
                    for (int j = 0; j < matrixDims; j++)
                    {
                        if (matrixPage1[i][j] != matrixPage2[j][i])
                        {
                            cout << "FALSE" << endl;
                            return;
                        }
                    }
                }
            }
        }
    }
    cout << "Number of blocks read:  " << blocksRead << endl;
    cout << "Number of blocks accessed:   " << blocksWritten + blocksRead << endl;
    cout << "TRUE" << endl;
    return;
}

void Matrix::renameMatrix(string renameTo)
{
    int blocksWritten = 0, blocksRead = 0;
    logger.log("Matrix::renameMatrix");
    string renameFrom = this->matrixName;
    matrixCatalogue.removeMatrix(renameFrom);
    this->matrixSourceFileName = "../data/"+renameTo+".csv";
    this->matrixName = renameTo;
    matrixCatalogue.insertMatrix(this);
    for (int i = 0; i < this->pages; i++)
    {
        for (int j = 0; j < this->pages; j++)
        {
            Page currPage = bufferManager.getMatrix(renameFrom, i, j);
            blocksRead++;
            string pagePath = gettableName(currPage);
            // cout<<pagePath<<endl;
            remove(pagePath.c_str());
            settableName(currPage, renameTo);
            setpageName(currPage, "../data/temp/" + gettableName(currPage) + "_matrix_page_" + getpageIndex(currPage) + "_" + getcolPageIndex(currPage));
            currPage.writeMatrixPage();
            blocksWritten++;
        }
    }
    cout << "Matrix " + renameFrom + " renamed to " + renameTo + " successfully" << endl;
    cout << "Number of blocks written:   " << blocksWritten << endl;
    cout << "Number of blocks read:  " << blocksRead << endl;
    cout << "Number of blocks accessed:   " << blocksWritten + blocksRead << endl;
}
/**
 * @brief Function to check if matrix is already exported
 *
 * @return true if exported
 * @return false otherwise
 */
bool Matrix::isPermanent()
{
    logger.log("Matrix::isPermanent");
    if (this->matrixSourceFileName == "../data/" + this->matrixName + ".csv")
    {
        return true;
    }
    return false;
}

void Matrix::writeDataToFile(vector<int> &data, ofstream &fout, int count, int flag)
{
    logger.log("Matrix::writeDataToFile");
    for (int col = 0; col < min(count, (int)data.size()); col++)
    {
        if (col != 0)
            fout << ", ";
        fout << data[col];
    }
    if (!flag)
        fout << ", ";
}

/**
 * @brief called when EXPORTMATRIX command is invoked to move source file to "data"
 * folder.
 *
 */
vector<int> Matrix::makePermanent(string exportMatrixDest, string fileName, int flag)
{
    int blocksWritten = 0, blocksRead = 0;
    logger.log("Table::makePermanent");
    if (!this->isPermanent())
        bufferManager.deleteFile(exportMatrixDest);
    string newSourceFile = exportMatrixDest;
    ofstream fout(newSourceFile, ios::out);
    int r = 0;
    for (; r < this->pages; r++)
    {
        int ppflag = 0;
        int maxRows = matrixDims;
        if (r == this->pages - 1)
        {
            maxRows = (this->rowsDim % matrixDims);
            if (maxRows == 0)
                maxRows = matrixDims;
        }
        for (int k = 0; k < matrixDims; k++)
        {
            int pflag = 0;
            int c = 0;
            for (; c < this->pages; c++)
            {
                int count = matrixDims;
                if (c == this->pages - 1)
                {
                    count = (this->rowsDim % matrixDims);
                    if (count == 0)
                        count = matrixDims;
                }
                if (r == this->pages - 1 && c == this->pages - 1)
                {
                    count = (this->rowsDim % matrixDims);
                    if (count == 0)
                        count = matrixDims;
                    if (k >= count - 1)
                    {
                        Cursor cursor(fileName, r, c);
                        blocksRead++;
                        vector<int> data = cursor.getMatrixPageRow(k);
                        int flag = 0;
                        if (c == this->pages - 1)
                            flag = 1;
                        writeDataToFile(data, fout, count, flag);
                        pflag = 1;
                        break;
                    }
                }
                if (r == this->pages - 1 && k >= maxRows)
                    break;
                Cursor cursor(fileName, r, c);
                blocksRead++;
                vector<int> data = cursor.getMatrixPageRow(k);

                int flag = 0;
                if (c == this->pages - 1)
                    flag = 1;
                writeDataToFile(data, fout, count, flag);
                blocksWritten++;
            }
            if (pflag)
            {
                ppflag = 1;
                break;
            }
            fout << endl;
        }
        if (ppflag)
        {
            break;
        }
    }
    fout.close();
    if (!flag)
    {
        cout << "Number of blocks read:  " << blocksRead << endl;
        cout << "Number of blocks accessed:   " << blocksRead << endl;
        return {0, 0};
    }
    return {blocksRead, 0};
}

/**
 * @brief Function to perform Matrix - Transpose(Matrix) operation
 *
 */
void Matrix::computeOnMatrix()
{
    int blocksWritten = 0, blocksRead = 0;
    for (int currRow = 0; currRow < this->pages; currRow++)
    {
        for (int currCol = currRow; currCol < this->pages; currCol++)
        {
            // diagonal page blocks.....
            if (currRow == currCol)
            {
                Page currPage = bufferManager.getMatrix(this->matrixName, currRow, currCol);
                blocksRead++;
                vector<vector<int>> currPageEle = currPage.getMatrixPageEle();
                for (int i = 0; i < matrixDims; i++)
                {
                    for (int j = i; j < matrixDims; j++)
                    {
                        if (i == j)
                        {
                            currPageEle[i][j] = 0;
                            continue;
                        }
                        currPageEle[i][j] = currPageEle[i][j] - currPageEle[j][i];
                        currPageEle[j][i] = -currPageEle[i][j];
                    }
                }
                bufferManager.writeMatrix_Page(this->matrixName + "_RESULT", currRow, currCol, currPageEle);
                blocksWritten++;
            }
            else
            {
                Page page1 = bufferManager.getMatrix(this->matrixName, currRow, currCol);
                Page page2 = bufferManager.getMatrix(this->matrixName, currCol, currRow);
                blocksRead += 2;
                vector<vector<int>> matrixPage1Data = page1.getMatrixPageEle();
                vector<vector<int>> matrixPage2Data = page2.getMatrixPageEle();

                for (int i = 0; i < matrixDims; i++)
                {
                    for (int j = 0; j < matrixDims; j++)
                    {
                        matrixPage1Data[i][j] = matrixPage1Data[i][j] - matrixPage2Data[j][i];
                        matrixPage2Data[j][i] = -matrixPage1Data[i][j];
                    }
                }

                bufferManager.writeMatrix_Page(this->matrixName + "_RESULT", currRow, currCol, matrixPage1Data);
                bufferManager.writeMatrix_Page(this->matrixName + "_RESULT", currCol, currRow, matrixPage2Data);
                blocksWritten += 2;
            }
        }
    }
    string computeResultDest = "../data/" + this->matrixName + "_RESULT.csv";
    vector<int> res = this->makePermanent(computeResultDest, this->matrixName + "_RESULT", 1);
    for (int i = 0; i < this->pages; i++)
    {
        for (int j = 0; j < this->pages; j++)
        {
            Page page = bufferManager.getMatrix(this->matrixName + "_RESULT", i, j);
            remove(gettableName(page).c_str());
        }
    }
    blocksWritten += res[1];
    blocksRead += res[0];
    cout << "Number of blocks written:   " << blocksWritten << endl;
    cout << "Number of blocks read:  " << blocksRead << endl;
    cout << "Number of blocks accessed:   " << blocksWritten + blocksRead << endl;
}

/**
 * @brief Function prints the first few rows of the matrix. If the table contains
 * more rows than PRINT_COUNT, exactly PRINT_COUNT rows are printed, else all
 * the rows are printed.
 *
 */
void Matrix::print()
{
    logger.log("Matrix::print");
    int blocksWritten = 0, blocksRead = 0;
    int pageRowIndex = 0, pageColIndex = 0;
    int cnt = 0;
    for (int pRow = 0; pRow < this->pages; pRow++)
    {
        int flag = 0;
        for (int r = 0; r < matrixDims; r++)
        {
            vector<int> row;
            int c = 0;
            while (row.size() < min((int)PRINT_COUNT, this->colsDim) && c < this->pages)
            {
                blocksRead++;
                Cursor cursor(this->matrixName, pRow, c);
                vector<int> data = cursor.getMatrixPageRow(r);
                row.insert(row.end(), data.begin(), data.end());
                c++;
            }
            // Print the Row
            for (int k = 0; k < min((int)PRINT_COUNT, this->colsDim); k++)
            {
                cout << row[k] << " ";
            }
            cout << endl;
            cnt++;
            if (cnt == min((int)this->rowsDim, (int)PRINT_COUNT))
            {
                flag = 1;
                break;
            }
        }
        if (flag)
            break;
    }
    cout << "Number of blocks read:  " << blocksRead << endl;
    cout << "Number of blocks accessed:   " << blocksWritten + blocksRead << endl;
}