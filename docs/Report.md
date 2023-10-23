# DATA SYSTEM- M'23
# Project Phase 1 - Report


## Detailed Report for each command we have implemented


### COMMAND 1: LOAD MATRIX


**Logic:**
- The LOAD command reads the CSV file consisting of matrix data and puts it into pages/blocks. Each block/page is of size 1024 Bytes in our system, which roughly means that it can hold integers of dimension 16*16. If the matrix CSV file is large, then data gets divided into multiple pages, which are indexed in a two dimensional fashion, the pages along a row will be inputMatrixDims%16 and number of rows will also be inputMatrixDims%16. Also, on executing the LOAD command the matrix object that is created, gets inserted in matrixCatalogue.


**Page Design:**
- The page size is of 1024 Bytes and we are storing data in every page in the form of square matrix of 16*16 integers. If the actual data is having less dimension than that of the page dimensions, then padding is done, to ensure that data dimension in a page remains in 16*16 format only.


**Number of Block Access Based on Input Size:**
- Data will written to the blocks, each block will have a dimension of 16*16. In case the data for a particular page is lesser than that, then padding will be done where 0 will be appended.


**Error Handling:**
- Syntactic parsing is done first to check if the command is typed correctly, followed by semantic parsing, which checks if the matrix file specified in the command exists or not and if the matrix is already loaded or not.




### COMMAND 2: PRINT MATRIX


**Logic:**
- The PRINT command prints the first 20 rows and columns of the matrix name specified in the command, if the size of the matrix is big (n >= 20), else it prints the whole square matrix data. It utilises cursors to fetch data from a page row by row. Here, we have implemented its logic in such a way that it prints the output line by line, starting from the first line of every page along the row, and using cursor to fetch the next lines.


**Page Design:**
- The page size is of 1024 Bytes and we are storing data in every page in the form of square matrix of 16*16 integers. If the actual data is having less dimension than that of the page dimensions, then padding is done, to ensure that data dimension in a page remains in 16*16 format only.


**Number of Block Access Based on Input Size:**
- We have used a 2 dimensional indexing scheme in page organisation for storing data of large size square matrices. Each line inside a page is accessed using a cursor for printing, until all the pages along the row are accessed, then we move on to the next row to access pages there and print their lines data. The number of blocks accessed is (number or rows in a page * number of blocks across the row). We analyzed that for 20x20 we are accessing each block for each row to be printed, hence more access. But we took the tradeoff as we have to then create more memory to store the elements we want to print if we want to reduce page accesses, and if in future more elements are to be printed we will require more storage to store to print the elements. So we took this tradeoff to access single row at a time from a block to reduce the storage.


**Error Handling:**
- Syntactic parsing is done first to check if the command is typed correctly, followed by semantic parsing, which checks if the matrix file specified in the command exists or not and if the matrix is present in matrixCatalogue.




### COMMAND 3: TRANSPOSE MATRIX


**Logic:**
- For transposing a matrix, we are using the deque which acts as the main memory, where at a time right now we are putting and accessing only two blocks. We are having each block of size 16x16 as each block can handle 1024B memory. So to transpose we are putting in at most 2 blocks in deque, that is, let’s say we have a 40x40 matrix and so 9 pages or blocks will be required. So while accessing diagonal blocks, we will first load it into main memory i.e. the deque available in BufferManager class and then we will do transpose into the block by swapping page[i][j] and page[j][i] with each other, here both i and j will go from 0 to rows-1, where rows are the number of rows and columns that can exist in a single page/ block, currently that is 16 in our case. And for non-diagonal blocks, we are getting the pages/ blocks like page1 = block[0][1] and page[2] = corresponding block[1][0] and then swapping first row of page1 and first column elements of page2 and then writing the updated pages back into memory i.e. temp folder in data folder. We are accessing pages into main memory in FIFO order, that is the order followed by the queue. 


**Page Design:**
- Page design remains the same as the page design mentioned in the LOAD COMMAND and while writing back the updated values the page design remains the same with only the elements being changed with their respective transposed elements. Page size being 1024B i.e. it can hold 16x16 elements in a page.


**Number of Block Access Based on Input Size:**
- Number of blocks read will be = square[ceil((number of rows in matrix)/(number of rows in a page))]
- Number of blocks written will be the same as the number of blocks read.
- So number of blocks accessed = 2*Number of blocks read


**Error Handling:**
- Semantic and Syntactic parsers are created and handled in respective cpp and header files. If the syntax of the commands mentioned in the project pdf are not followed then “SYNTAX ERROR” will be shown. And if the matrix that the user wants to transpose is already not loaded into memory, that is, not available in MatrixCatalogue then a semantic error message will be shown. The page needs to be accessed if it is already not inPool or in deque then we will load it only if the matrix is loaded or available in MatrixCatalogue. 




### COMMAND 4: EXPORT MATRIX


**Logic:**
- The EXPORT command takes page’s data from the temp folder for a particular matrix and puts it into the associated matrix file in the data folder. The lines of each page along the rows are accessed, they are appended and then they are inserted into the matrix file in the data folder.


**Page Design:**
- The page size is of 1024 Bytes and we are storing data in every page in the form of square matrix of 16*16 integers. If the actual data is having less dimension than that of the page dimensions, then padding is done, to ensure that data dimension in a page remains in 16*16 format only.


**Number of Block Access Based on Input Size:**
- We have used a 2 dimensional indexing scheme in page organisation for storing data of large size square matrices. Each line inside a page is accessed using a cursor for printing, until all the pages along the row are accessed, then we move on to the next row to access pages there and print their lines data.


**Error Handling:**
- Syntactic parsing is done first to check if the command is typed correctly, followed by semantic parsing, which checks if the matrix file specified in the command exists or not and if the matrix is present in matrixCatalogue.




### COMMAND 5: RENAME MATRIX


**Logic:**
- For renaming the matrix, we are accessing the page object and loading it into deque and then we are accessing the metadata of the page. We are storing the previous metadata for deletion of previously named pages in temporary variables. After that we changed the metadata, like changing the tableName which is the matrixName and we change the pageName which stores the path of the file when it is stored in the temp folder. So we change the metadata and then the pages are deleted using the previous metadata info stored, then we write the page back into memory or temp folder.


**Page Design:**
- Page design remains the same as when we LOAD the page into memory. Page of a matrix stores the following metadata:
string tableName : the name of the matrix to which the page belongs. This is what we change while renaming.
string pageIndex : The rowIndex of the block, that we get after performing ceil((number of rows in matrix)/(number of rows in a page)
string colPageIndex : The columnIndex of the block, same range of values as pageIndex.
int columnCount : total number of columns in actual matrix
int rowCount : total number of rows in actual matrix
vector<vector<int>> rows : elements stored in this block/ page of the matrix 


**Number of Block Access Based on Input Size:**
- Number of blocks read will be = square[ceil((number of rows in matrix)/(number of rows in a page))]
- Number of blocks written will be the same as the number of blocks read, as the blocks read will be deleted and new blocks with updated metadata are written back into memory.
- So number of blocks accessed = 2*Number of blocks read


**Error Handling:**
- Both semantic parsing and syntactic parsing is also done for this command as well, with respective functions as semanticParseRENAME() and syntacticParseRENAME(). Any error encountered and then code will stop further execution.




### COMMAND 6: CHECKSYMMETRY


**Logic:**
- The logic is similar to the transpose command. We are accessing a single page at a time for diagonal matrices and comparing each row of the page with each column of the page. And for non-diagonal pages we are both the respective pages, for example (0,1) and (1,0) pages and then comparing each row of page1 with each column of page2. Even if a single element is not the same as their respective, we return FALSE and if every element follows a symmetric condition we return TRUE.


**Page Design:**
- Since we are not writing or updating the pages, the page design remains the same as we have LOADED it into the memory.


**Number of Block Access Based on Input Size:**
- Number of blocks accessed = number of blocks read = square[ceil((number of rows in matrix)/(number of rows in a page))]


**Error Handling:**
- semanticParseCHECKSYMMETRY() and syntacticParseCHECKSYYMMETRY() functions have been created to perform semantic and syntactic checking respectively.




### COMMAND 7: COMPUTE


**Logic:**
- We have used a 2 dimensional page indexing scheme. So for executing the COMPUTE command which basically performs the operation of subtraction(MATRIX, TRANSPOSE(MATRIX)), we access the pages indexed (i, j) and (j, i) and access their elements and perform the operation of them. The (i1, j1)th element of page (i,j) will be subtracted from (j1,i1)th element of page(j, i) and vice-versa. 


**Page Design:**
- Describe the design of the page associated with this command (if applicable).


**Number of Block Access Based on Input Size:**
- The number of blocks/pages accessed at a time will only be 2, as at a time we will only require pages indexed (i,j) and (j,i) for the COMPUTE command operation.


**Error Handling:**
- Syntactic parsing is done first to check if the command is typed correctly, followed by semantic parsing, which checks if the matrix file specified in the command exists or not and if the matrix is present in matrixCatalogue.




## Assumptions
- The dequeue size which acts as main memory should have a minimum size of 2 or more.
- The block size currently is taken as 1024B, which means 16x16 values will be there, but even if the block size is more or less, it will work.
- The values in the matrix will be integers only and no underflow or overflow will occur.
- For commands, like EXPORT we are assuming that if there already exists the same file name then it will be overwritten.
- We can create our own constructor for Page that can store the metadata for the matrix pages.
- Only square matrices will be given as input.
- No table and matrix will have the same name.




## Learnings


- Our major learnings and takeaways from this Project phase 1 are:
How paging is being done and how an efficient page structure for any use-case is created, like in our project we have to design a page structure that can help us efficiently perform the given functionalities.
We learned from the cursor how we can access pages for printing and other commands.
We also learned how paging works in main memory as main memory is limited and we have kept checks in main memory, that is our deque in bufferManager and the page not in our deque we cannot directly access it. So we first put the page in deque and then directly access it.
We learned to store pages in main memory, how page in and page out operations are performed. In our case we had a deque and followed FIFO policy to perform paging.
From renaming, we understood how metadata can be accessed and changed for a page, to perform rename like operations.




## Contribution of Each Member


- Siddhant Gupta (Roll No.- 2022201037): Coded the functionalities of LOAD MATRIX, PRINT MATRIX and EXPORT commands. Also contributed to the report.
- Mayush Kumar (Roll No.- 2022201043): Coded the functionalities of both TRANSPOSE MATRIX and RENAME command. Also contributed to the report.
- Mohit Sharma (Roll No.- 2022201060): Coded the functionalities of CHECKSYMMETRY and COMPUTE command. Also contributed to the report for the following commands.




## Coordination as a Team


- Our team coordinated very well, and we first started by each member understanding the flow of the code. And then we sat down together and agreed upon the most efficient page design for the given commands. We then worked on our respective commands and then shared our logics on how we will be implementing our commands, and as a team we agreed upon the correct logics to simulate the concepts of paging as 	closely as possible. As soon as commands were getting done, we were pushing it onto github, so other team members can take help from it if needed.