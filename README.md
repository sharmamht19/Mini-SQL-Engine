# Mini SQL Engine

The Mini SQL Engine, codenamed "simpleRA", is a streamlined, integer-only relational database management system (RDBMS). While not as intricate as market-standard DBMS systems, simpleRA offers a compact yet efficient database solution implemented in C/C++.

---

## Table of Contents

- [Brief Overview](#brief-overview)
- [Compilation Instructions](#compilation-instructions)
- [Architecture](#architecture)
- [Features](#features)
  * [Assignment Statements](#assignment-statements)
  * [Non-Assignment Statements](#non-assignment-statements)
- [Implementation Details](#implementation-details)
  * [EXTERNAL SORTING](#external-sorting)
  * [JOIN](#join)
  * [ORDER BY](#order-by)
  * [GROUP BY](#group-by)
- [Relational Algebra Grammar](#relational-algebra-grammar)
- [Limitations](#limitations)
- [Assumptions](#assumptions)
- [Learnings](#learnings)
- [Challenges](#challenges)
- [License](#license)
- [Contribution](#contribution)

---

## Brief Overview

SQL engines are pivotal components in recognizing and interpreting SQL commands for data interrogation within a relational database. The simpleRA, while limited in its feature set and syntax deviation from traditional SQL queries, emulates the procedural aspects of query execution. Unlike extensive SQL servers, it refrains from handling nested queries.

## Compilation Instructions

We use ```make``` to compile all the files and creste the server executable. ```make``` is used primarily in Linux systems, so those of you who want to use Windows will probably have to look up alternatives (I hear there are ways to install ```make``` on Windows). To compile

```cd``` into the SimpleRA directory
```
cd SimpleRA
```
```cd``` into the soure directory (called ```src```)
```
cd src
```
#### To compile

```bash
make clean
make
```

#### To run

Post compilation, an executable names ```server``` will be created in the ```src``` directory
```bash
./server
```
## Architecture

The core of simpleRA comprises of:
- **Buffer Manager**: Manages the data transfer between in-memory and disk storage.
- **Tables**: The structural foundation to store data.
- **Cursors**: Enables traversal and interaction within the tables.
- **Executors**: Instrumental in executing parsed SQL commands.

User interactions transpire through a two-fold process: 
1. **Parsing Stage**: The RDBMS initializes the process by parsing the SQL statement via syntactic and semantic parsers, ensuring syntax and semantic correctness.
2. **Execution Stage**: The RDBMS proceeds to actualize the SQL statement utilizing executors.

## Features 

 Mini SQL Engine facilitates a multitude of SQL commands, categorized into assignment and non-assignment statements. A B+ tree indexing strategy enhances the speed of range queries

### Assignment Statements

Resultant in new table creations, these include:
- SELECTION
- PROJECTION
- CROSS
- JOIN
- SORT
- GROUP BY
- ORDER BY
- DISTINCT

### Non-Assignment Statements

Operations occur within the existing table:
- LOAD
- LIST
- PRINT
- RENAME
- UPDATE
- EXPORT
- CLEAR
- QUIT

## Implementation Details

### EXTERNAL SORTING

**Approach:**  
- Start with initializing sorting parameters from parsed query.
- For each chunk of data in table (chunk size based on max rows per block):
  - Sort data in-memory using given criteria.
  - Write sorted data back to the table.
- Use `externalSort()` for further sorting and merging.
- Finalize sorted data.

### JOIN

**Approach:**  
- Extract table names, join columns, and binary operator from parsed query.
- Retrieve target tables from the Table Catalogue.
- Create temporary tables and apply external sorting on join columns.
- Create result table for storing JOIN operation results.
- For each row in sorted tables:
  - Compare and join based on provided binary operator.
  - Store result in result table.
- Finalize result table with updated metadata.

### ORDER BY

**Approach:**  
- Create `sortedTable` for holding sorted data.
- Copy data from original table to `sortedTable`.
- Apply external sorting on `sortedTable` based on specified attributes.
- Load sorted data back into table for further operations.

### GROUP BY

**Approach:**  
- Extract group attributes, aggregation functions, and HAVING clause details from query.
- Create and sort a temporary table based on group attribute.
- For each row in sorted table:
  - Group data and apply aggregation functions.
  - If HAVING condition is met, store result in `groupedTable`.
- Write final `groupedTable` results to a CSV file.

### PAGING

**Approach:**  
- Divide the system memory into blocks of 1024 Bytes each, referred to as pages.
- Convert matrices to a size that fits within the page limit. For instance, a 16x16 matrix.
- For larger matrices, decompose them into multiple 16x16 matrices, effectively storing them across various pages.
- Organize these pages in a two-dimensional structure, facilitating retrieval and modifications.

### BUFFER MANAGEMENT

**Approach:**  
- Implement a deque (double-ended queue) structure to represent the main memory buffer.
- As new pages are loaded into memory, older pages (based on the FIFO policy) are evicted when the buffer limit is reached.
- When retrieving a matrix, check the buffer for the respective pages. If not present, load the necessary page into the buffer and evict the oldest page.

### ERROR HANDLING

**Approach:**  
- Before executing a command, perform a syntactic check to verify the command's structure and arguments.
- If the syntactic check passes, proceed to the semantic check to ensure the operation's logical validity.
- In case of an error, generate descriptive error messages to guide users to correct their input.

### LOAD MATRIX

**Approach:**  
- Read the matrix data from the specified CSV file.
- Divide the matrix into 16x16 chunks, or pages.
- Load these pages into the memory buffer, evicting older pages if necessary following the FIFO policy.

### PRINT MATRIX

**Approach:**  
- If the matrix size is smaller than 20x20, display the entire matrix.
- If larger, extract and display only the first 20 rows and columns.
- Provide visual indicators if a matrix is truncated during display.

### TRANSPOSE MATRIX

**Approach:**  
- Using the memory buffer, retrieve matrix pages one by one.
- Transpose each 16x16 matrix page in the buffer.
- Store the transposed matrix back into the memory.

### EXPORT MATRIX

**Approach:**  
- Retrieve the matrix from the memory pages.
- Construct a complete matrix, combining pages if necessary.
- Write the matrix data into the desired file format, such as CSV.

### RENAME MATRIX

**Approach:**  
- Identify the matrix using its current name or metadata.
- Update its name in the metadata and other associated references.
- Ensure the renaming doesn't cause conflicts with existing matrix names.

### CHECKSYMMETRY

**Approach:**  
- Extract matrix pages from memory and construct the matrix.
- Compare each element (i, j) with the element (j, i) for the entire matrix.
- If all such pairs match, the matrix is symmetric; otherwise, it's not.

### COMPUTE

**Approach:**  
- Based on the specified operation, retrieve the necessary matrices from memory.
- Perform the matrix operations, such as subtraction, multiplication, etc.
- Store the result back into the memory, possibly evicting older pages if the buffer gets full.

## Relational Algebra Grammar

```
Statement -> relation_name <- assignment_statement
           | non_assignment_statement

assignment_statement -> cross_product_statement
                      | distinct_statement
                      | join_statement
                      | projection_statement
                      | selection_statement
                      | sort_statement
                       
non_assignment_statement -> clear_statement 
                           | index_statement
                           | list_statement
                           | load_statement
                           | print_statement
                           | quit_statement
                           | rename_statement
                           | source_statement

cross_product_statement -> CROSS relation_name relation_name

distinct_statement -> DISTINCT relation_name

join_statement -> JOIN relation_name, relation_name ON column_name bin_op column_name

projection_statement -> PROJECT projection_list FROM relation_name

projection_list -> projection_list, column_name 
                 | column_name

selection_statement -> SELECT condition FROM relation_name

condition -> column_name binop column_name 
           | column_name binop int_literal

binop -> > | < | == | != | <= | >= | => | =< 

sort_statement -> SORT relation_name BY column_name IN sorting_order

sorting_order -> ASC | DESC

clear_statement -> CLEAR relation_name

index_statement -> INDEX ON column_name FROM relation_name USING indexing_strategy

indexing_strategy -> HASH | BTREE | NOTHING;

list_statement -> LIST TABLES;

load_statement -> LOAD relation_name

print_statement -> PRINT relation_name

quit_statement -> QUIT

rename_statement -> RENAME column_name TO column_name FROM relation_name

source_statement -> SOURCE file_name

```
## Limitations

1. Relational Algebra Operators
2. Integers Only
3. No update operations
4. No aggregate operations
5. No nested queries
6. No transaction management
7. Single thread programming only
8. No identifiers should have spaces in them

   
## Assumptions

- Comparisons and sorts are on integers (4 Bytes each). Support for larger strings can be added by adjusting byte size.
- Cursors retrieve integer data.
- Syntax follows given document standards, including spaces.
- Users are responsible for deleting resultant tables. Overwriting isn't permitted.
- No integer overflow for SUM operations.
- Each page has a fixed size of 1024 Bytes.
- Matrices are stored as 16x16 chunks in pages.
- Commands follow the specified syntax and semantics for proper functioning.
- Matrices are distinct by names. Overwriting isn't permitted.
- Larger matrices are decomposed into smaller pages and retrieved accordingly for operations.


## Learnings

1. Real-world application of external sorting and K-way merge.
2. Efficient implementation of JOIN, GROUP BY, and ORDER BY commands using external sorting.
3. Management of large files and partial data access.
4. Data persistence requires metadata and data updates on disk.
5. Efficient data read-write with BufferManager.
6. Efficient handling of matrix operations and transformations using a memory buffer and paging mechanism.
7. Implementation of LOAD, PRINT, TRANSPOSE, and other commands utilizing the buffer management and error handling.
8. Management and organization of matrices across multiple pages.
9. Data persistence requires metadata management and updates in the system's memory.
10. Effective interaction with external file formats like CSV during matrix import and export.


## Challenges

During the development phase, the team encountered several challenges:
- Loading extensive tables into main memory for operations.
- Numerous code revisions during the B+ tree indexing implementation to accommodate edge test cases
- Properly segmenting and storing larger matrices across multiple pages.
- Handling matrix transformations and operations when matrices span multiple pages.
- Ensuring error handling covers a wide range of possible user inputs.
- Balancing between buffer size and matrix size for optimized performance.
- Adapting to matrices of varying sizes and ensuring seamless operation execution.

## Contribution

Feel free to explore the source code repository, watch the demo video, and refer to the communication and architecture diagrams to get a better understanding of the `Mini SQL Engine`.

## License

MIT License.

