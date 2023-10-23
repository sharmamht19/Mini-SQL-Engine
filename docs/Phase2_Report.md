# DATA SYSTEM- M'23
# Project Phase 2 - Report
# Team - 36

## Detailed Report for each command we have implemented

## EXTERNAL SORTING 

### `sortTable()`

#### Overview:
The `sortTable()` function is a part of the Table class and is responsible for sorting the data within the table based on specified columns and sort orders. This function is essential for maintaining the order of data in the table, facilitating efficient query processing.

#### Implementation:

**Sorting Criteria:**
- The function starts by initializing sorting parameters, such as sorting values and column indexes, based on the parsed query.

**Data Retrieval:**
- The function uses a cursor to retrieve data from the table in chunks, where each chunk corresponds to the maximum rows per block.

**In-Memory Sorting:**
- Data retrieved in each chunk is sorted in-memory using the `sortComparator` function, which is a custom comparator based on specified columns and sort orders.

**Page Writing:**
- Sorted chunks are then written back to the table, ensuring the overall data is now sorted based on the specified criteria.

**External Sorting:**
- After in-memory sorting, the function calls the `externalSort()` method for further sorting and merging, particularly useful for large datasets that do not fit entirely in memory.

**Finalization:**
- The sorted data is made permanent, and the function concludes its execution.

### `externalSort()`

#### Overview:
The `externalSort()` function is responsible for performing an external sort on the table data. External sorting is crucial when the dataset is too large to fit entirely in memory, requiring a multi-step merging approach.

#### Implementation:

**Merge Iterations Calculation:**
- The number of merge iterations is calculated based on the logarithm of the block count and the maximum block count allowed.

**Cluster Creation:**
- The table blocks are divided into clusters, and each cluster's boundaries are determined.

**K-Way Merge:**
- The function performs a K-way merge by iteratively merging clusters of blocks. It uses cursors to navigate through the blocks, maintaining sorted order during the merge. In our case we used k = 9, as BLOCK SIZE = 10, 1 block for buffer to write back into memory.

**Page Writing:**
- Merged data is written to temporary files, representing the merged pages.

**Final Merge:**
- After completing all iterations, the temporary files are merged into the final sorted order.


## JOIN

### `joinTables()`

#### Overview:
The `joinTables()` function in the Table class is responsible for performing a JOIN operation between two tables based on specified conditions. This function utilizes external sorting to efficiently handle large datasets that may not fit entirely in memory.

#### Implementation:

1. **Initialization:**
   - The function starts by extracting information from the parsed query, including the names of the tables to be joined, the columns for the join operation, and the binary operator.

2. **Table Retrieval:**
   - Tables to be joined are retrieved from the Table Catalogue using their names obtained from the parsed query.

3. **Temporary Tables Creation:**
   - Two temporary tables (`temp_GroupBy1` and `temp_GroupBy2`) are created to store the sorted data from each of the tables to be joined. This is done as we don't want to inplace sort the original files.

4. **External Sorting:**
   - External sorting is applied to both temporary tables using the `sortTable()` function. Sorting is performed based on the specified join columns.

5. **JOIN Operation:**
   - A new table (`resultTable`) is created to store the result of the JOIN operation which is provided us when the command is given. We create new pages for the join data and when the max rows per block is reached we will create new page for the `resultTable`.
   - Join function iterates through the sorted data of both tables, comparing rows based on the specified binary operator.
   - When a matching condition is met, rows are combined, and the result is stored in the pages of `resultTable`.

6. **Page Writing**
   - Merged rows are written to pages in the `resultTable`, that we got while the command is given.

7. **Result Table Finalization:**
   - The result table is finalized, and metadata such as row count and column count are updated.


## ORDERBY

### `orderBy()`

#### Overview:
The `orderBy()` function in the Table class is designed to order the rows of a table based on a specified attribute and sorting strategy. This function employs an external sorting algorithm to handle large datasets efficiently.

#### Implementation:

1. **Temporary Table Creation:**
   - A new table (`sortedTable`) is created to store the sorted data. This table has the same structure as the original table. We get this table as a part of our command.

2. **Data Copy to Temporary Table:**
   - The function uses a cursor to iterate through the original table's rows and copies them to the `sortedTable`.

3. **External Sorting:**
   - External sorting is applied to the `sortedTable` using the `sortTable()` function. Sorting is based on the specified order attribute and sorting strategy.

4. **Result Table Loading:**
   - The sorted data is loaded back into the tableCatalogue, ensuring that the table is accessible for further operations.


## GROUPBY

### `groupBy()`

#### Overview:
The `groupBy()` function in the Table class is responsible for performing a GROUP BY operation on a specified attribute, applying an aggregation function, and an optional HAVING clause. This implementation uses external sorting for efficient processing of large datasets.

#### Implementation:

1. **Initialization:**
   - Extract necessary information from the parsed query, including the group and aggregate attributes, aggregation functions, HAVING clause details, and result table information.

2. **External Sorting for GROUP BY:**
   - Create a temporary table (`sortedTable`) and use external sorting based on the group attribute.

3. **Result Table Creation:**
   - Create a new table (`groupedTable`) to store the result of the GROUP BY operation.

4. **Grouping and Aggregation:**
   - Iterate through the sorted data and perform grouping and aggregation based on the specified attributes and functions. AS long as the condition is correct we aggreagate both the having condition attribute and return attribute based on what type of Aggregate operation both have and then we write into the page of `resultTable`. We did checking and writing the result in single pass.

5. **File Output:**
   - Write the result table to a CSV file for persistence by reading all it's pages.


## Assumptions

1. All the comparisons and sorting will be done on integer that's why we have taken the size of each value as 4 Bytes, so string of 4 Bytes will be supported, and sorting will work on larger strings by increasing the size of each elements.

2. We are assuming that since the cursor gets integer data only so while doing AVG operation we are only storing the integers.

3. Since we don't have != operator given in the project document, therefore we didn't implement it.

4. Syntax needs to be followed as given in project phase 2 document (including spaces).

5. All the resultant table will have to be deleted by user, we doesn't allow user to overwrite the resultant table.

6. Sum will not have integer overflow.


## Learnings

1. Understanding of how external sorting works in real life databases adn how to effeciently implement it using K-way merge by using the concepts of paging, as in our case we got 9 way merge due to BLOCK SIZE = 10. 

2. We also got to know how to effeciently implement JOIN, GROUP BY, ORDER BY commands by using external sorting.

3. We also learn how does all this happen when files are of large size (> main memory) and we can only access some pages at a time.

4. We also understand that to make the data persistent, we have to update the metadata of the files and data of the files in our disk by bringing them into main memory.

5. We also learned how to use BufferManger to effeciently read and write data into memory.



## Contribution and Coordination as a team
We held mutiple meetings to discuss the logic of each functions and we also meet when anyone of our team mate got stuck while implementing. In our first meeting we discuss the logic on how we will implement the external sorting. In our next meeting we defined the algorithm that we will use to implement sorting and other commands that needs to build over the top of it. All the logics that we have implemented are the result of brainstorming sessions that we did as a team, so each one of us know how each logic works.
We then took charge of implementing the functions, and below is who did code for what part:
<br>
*Siddhant Gupta (Roll No.- 2022201037) - EXTERNAL SORTING*
<br>
*Mohit Sharma (Roll No.- 2022201060) - JOIN, ORDER BY*
<br>
*Mayush Kumar (Roll No.- 2022201043) - GROUP BY*
