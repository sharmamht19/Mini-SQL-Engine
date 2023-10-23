Mini SQL Engine

The Mini SQL Engine, codenamed "simpleRA", is a streamlined, integer-only relational database management system (RDBMS). While not as intricate as market-standard DBMS systems, simpleRA offers a compact yet efficient database solution implemented in C/C++.

---

## Table of Contents

- [Brief Overview](#brief-overview)
- [Architecture](#architecture)
- [Features](#features)
  * [Assignment Statements](#assignment-statements)
  * [Non-Assignment Statements](#non-assignment-statements)
- [Challenges](#challenges)
- [Implementation Details](#implementation-details)
  * [EXTERNAL SORTING](#external-sorting)
  * [JOIN](#join)
  * [ORDER BY](#order-by)
  * [GROUP BY](#group-by)
- [Assumptions](#assumptions)
- [Learnings](#learnings)
- [Team Contributions](#team-contributions)

---

## Brief Overview

SQL engines are pivotal components in recognizing and interpreting SQL commands for data interrogation within a relational database. The simpleRA, while limited in its feature set and syntax deviation from traditional SQL queries, emulates the procedural aspects of query execution. Unlike extensive SQL servers, it refrains from handling nested queries.

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

simpleRA facilitates a multitude of SQL commands, categorized into assignment and non-assignment statements. A B+ tree indexing strategy enhances the speed of range queries.

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

### Assumptions

- Comparisons and sorts are on integers (4 Bytes each). Support for larger strings can be added by adjusting byte size.
- Cursors retrieve integer data.
- Syntax follows given document standards, including spaces.
- Users are responsible for deleting resultant tables. Overwriting isn't permitted.
- No integer overflow for SUM operations.

### Learnings

1. Real-world application of external sorting and K-way merge.
2. Efficient implementation of JOIN, GROUP BY, and ORDER BY commands using external sorting.
3. Management of large files and partial data access.
4. Data persistence requires metadata and data updates on disk.
5. Efficient data read-write with BufferManager.

## Challenges

During the development phase, the team encountered several challenges:
- Loading extensive tables into main memory for operations.
- Numerous code revisions during the B+ tree indexing implementation to accommodate edge test cases.

