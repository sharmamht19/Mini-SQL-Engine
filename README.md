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

## Challenges

During the development phase, the team encountered several challenges:
- Loading extensive tables into main memory for operations.
- Numerous code revisions during the B+ tree indexing implementation to accommodate edge test cases.

