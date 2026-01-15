# User Guide

> Database Management System (DBMS) - User Operation Guide

## Table of Contents

1. [Introduction](#introduction)
2. [Getting Started](#getting-started)
3. [Table Management](#table-management)
4. [Data Operations](#data-operations)
5. [SQL Execution](#sql-execution)
6. [Index Management](#index-management)
7. [Keyboard Shortcuts](#keyboard-shortcuts)
8. [Troubleshooting](#troubleshooting)

---

## Introduction

The Database Management System (DBMS) is a custom database management system designed for managing data in a general-purpose manner. It provides a graphical user interface for table structure management, data operations, SQL query execution, index management, and query optimization.

### Key Features

- **Table Structure Management**: Create, edit, rename, and delete database tables
- **Data Operations**: Insert, update, delete, and view records
- **SQL Query Execution**: Execute DDL, DML, and SELECT queries
- **Index Management**: Create and manage indexes (Hash Index, Adjacent Index, B+Tree Index) for query optimization
- **Query Optimization**: Intelligent index recommendations and query performance analysis
- **Transaction Management**: Support for transactions with ACID properties (planned)
- **View Management**: Create and manage database views (planned)
- **User and Permission Management**: User authentication and access control (planned)

---

## Getting Started

### Launching the Application

1. Run the executable file: `MusicDBMS.exe`
2. The main window will open with five tabs:
   - **Table Management**: Manage database table structures
   - **Index Management**: Manage database indexes
   - **Data Operation**: Insert, update, and delete records
   - **SQL Execution**: Execute SQL statements
   - **Guide**: This user guide

### Database Management

Before working with tables, you need to create or open a database:

**Creating a New Database:**
1. Click **File** -> **Create Database** (or press `Ctrl+N`)
2. In the file dialog, choose a location and enter a database file name
3. The system will automatically create `.dbf` and `.dat` files
4. The database will be loaded automatically

**Opening an Existing Database:**
1. Click **File** -> **Open Database** (or press `Ctrl+O`)
2. In the file dialog, select a `.dbf` file
3. The database will be loaded and displayed in the status bar

**Current Database:**
- The current database name is displayed in the status bar (bottom right)
- All operations (table creation, data operations, SQL execution) use the current database
- To switch databases, create or open a different database

### Database Files

The system uses two types of files:
- **`.dbf` files**: Store table structure definitions
- **`.dat` files**: Store table record data

Both files use the same base name (e.g., `MusicDB.dbf` and `MusicDB.dat`).

---

## Table Management

### Creating a Table

**Method 1: Using GUI**

1. Navigate to the **Table Management** tab
2. Click the **Create Table** button
3. Enter the database file name (without extension)
4. Enter the table name
5. Define fields:
   - Field name
   - Field type (int, char, string, float, double)
   - Key flag (KEY or NOT_KEY)
   - Null flag (NULL or NO_NULL)
   - Valid flag (VALID or INVALID)
6. Click **Create**

**Method 2: Using SQL**

```sql
CREATE TABLE Songs (
    SongID int KEY NO_NULL VALID,
    SongName char[100] NOT_KEY NO_NULL VALID,
    Artist char[50] NOT_KEY NULL VALID,
    Genre char[30] NOT_KEY NULL VALID,
    Year int NOT_KEY NULL VALID,
    Duration int NOT_KEY NULL VALID
) INTO MusicDB;
```

### Viewing Tables

1. Navigate to the **Table Management** tab
2. Select a table from the table list (left panel)
3. View table structure in the details panel (right panel):
   - Table name
   - Field count
   - Detailed field information (name, type, size, KEY, NULL, VALID flags)

### Editing Table Structure

1. Navigate to the **Table Management** tab
2. Select a table from the table list
3. Click the **Edit** button
4. In the "Edit Table" dialog:
   - Modify table name (if needed)
   - Add, remove, or modify fields
   - Reorder fields using **Move Up** and **Move Down**
5. Click **Edit** to save changes
6. **Note**: Clicking the X button will prompt for confirmation before closing

### Deleting a Table

1. Navigate to the **Table Management** tab
2. Select a table from the table list
3. Click the **Delete** button
4. Confirm deletion in the dialog
5. The table will be removed from the database

### Refreshing the Table List

1. Navigate to the **Table Management** tab
2. Click the **Refresh** button
3. The table list will be updated with the latest tables from the current database

### Renaming a Table

**Using SQL:**

```sql
RENAME TABLE OldTableName NewTableName IN MusicDB;
```

### Deleting a Table

**Using SQL:**

```sql
DROP TABLE TableName IN MusicDB;
```

---

## Data Operations

The **Data Operation** tab allows you to insert, view, edit, and delete records in database tables.

### Selecting a Table

1. Navigate to the **Data Operation** tab
2. Select a table from the **Table** dropdown
3. The table's records will be automatically displayed in the data table below
4. The status bar shows the table name and record count

**Note**: The table list is automatically refreshed when you:
- Switch to the Data Operation tab
- Create a new table in the Table Management tab (switch to Data Operation tab to see it)
- Open a database

### Viewing Records

1. Navigate to the **Data Operation** tab
2. Select a table from the dropdown
3. All valid records will be displayed in a table view:
   - Each row represents one record
   - Each column represents one field
   - Key fields are marked with "(KEY)" in the column header
   - The table shows only valid (non-deleted) records

### Inserting Records

**Method 1: Using GUI**

1. Navigate to the **Data Operation** tab
2. Select a table from the dropdown
3. Click the **Insert** button
4. In the "Insert Record" dialog:
   - Enter values for each field
   - Fields marked with "*" are required (NOT NULL)
   - Fields marked with "(KEY)" are key fields
   - The system validates data types automatically
5. Click **Insert** to save the record
6. The record will be added and the table will refresh automatically

**Method 2: Using SQL**

```sql
INSERT INTO Songs VALUES (1, 'Bohemian Rhapsody', 'Queen', 'Rock', 1975, 355) IN MusicDB;
```

### Editing Records

**Method 1: Using GUI**

1. Navigate to the **Data Operation** tab
2. Select a table from the dropdown
3. Select a record by clicking on a row in the data table
4. Click the **Edit** button
5. In the "Edit Record" dialog:
   - Modify field values as needed
   - The system validates data types and constraints
6. Click **Save** to save changes
7. The record will be updated and the table will refresh automatically

**Method 2: Using SQL**

```sql
UPDATE Songs (
    SET Genre='Classic Rock'
    WHERE SongID=1
) IN MusicDB;
```

### Deleting Records

**Method 1: Using GUI**

1. Navigate to the **Data Operation** tab
2. Select a table from the dropdown
3. Select a record by clicking on a row in the data table
4. Click the **Delete** button
5. Confirm deletion in the dialog
6. The record will be marked as invalid (soft delete) and the table will refresh automatically

**Method 2: Using SQL**

```sql
DELETE FROM Songs WHERE SongID=1 IN MusicDB;
```

### Refreshing Data

1. Click the **Refresh** button to reload the current table's records
2. Or switch to another tab and back to automatically refresh

---

## SQL Execution

### Executing SQL Statements

1. Navigate to the **SQL Execution** tab
2. Enter SQL statement in the text editor
3. Click **Execute** button (or press `Ctrl+Enter`)
4. View results in the results panel
5. Check error messages if execution fails

### Supported SQL Syntax

#### DDL (Data Definition Language)

**CREATE TABLE:**
```sql
CREATE TABLE TableName (
    FieldName1 Type1 KEY_Flag1 NULL_Flag1 VALID_Flag1,
    FieldName2 Type2 KEY_Flag2 NULL_Flag2 VALID_Flag2,
    ...
) INTO DatabaseFileName;
```

**EDIT TABLE:**
```sql
EDIT TABLE TableName (
    FieldName Type KEY_Flag NULL_Flag VALID_Flag
) IN DatabaseFileName;
```

**RENAME TABLE:**
```sql
RENAME TABLE OldTableName NewTableName IN DatabaseFileName;
```

**DROP TABLE:**
```sql
DROP TABLE TableName IN DatabaseFileName;
```

#### DML (Data Manipulation Language)

**INSERT:**
```sql
INSERT INTO TableName VALUES (value1, value2, ...) IN DatabaseFileName;
```

**UPDATE:**
```sql
UPDATE TableName (
    SET Field1=Value1
    WHERE Field2=Value2
) IN DatabaseFileName;
```

**DELETE:**
```sql
DELETE FROM TableName WHERE Field=Value IN DatabaseFileName;
```

#### Query

**SELECT (Single Table):**
```sql
SELECT * FROM TableName WHERE Field=Value;
```

**SELECT (Multiple Tables):**
```sql
SELECT * FROM Table1, Table2 WHERE Table1.Field1=Table2.Field2;
```

**SELECT (JOIN):**
```sql
SELECT * FROM Table1 INNER JOIN Table2 ON Table1.Field1=Table2.Field2;
SELECT * FROM Table1 LEFT JOIN Table2 ON Table1.Field1=Table2.Field2;
SELECT * FROM Table1 RIGHT JOIN Table2 ON Table1.Field1=Table2.Field2;
SELECT * FROM Table1 FULL OUTER JOIN Table2 ON Table1.Field1=Table2.Field2;
SELECT * FROM Table1 FULL JOIN Table2 ON Table1.Field1=Table2.Field2;  -- OUTER is optional
SELECT * FROM Table1 NATURAL JOIN Table2;  -- Automatic join on common columns
SELECT * FROM Table1 NATURAL LEFT JOIN Table2;
SELECT * FROM Table1 NATURAL RIGHT JOIN Table2;
SELECT * FROM Table1 NATURAL INNER JOIN Table2;
SELECT * FROM Table1 NATURAL FULL JOIN Table2;
```

**SELECT (ORDER BY, DISTINCT, LIMIT):**
```sql
SELECT * FROM TableName ORDER BY FieldName ASC;
SELECT * FROM TableName ORDER BY FieldName ASC, FieldName2 DESC;  -- Multiple fields
SELECT DISTINCT FieldName FROM TableName;
SELECT * FROM TableName LIMIT 10;
SELECT DISTINCT FieldName FROM TableName ORDER BY FieldName DESC LIMIT 5;
```

**SELECT (Complex WHERE):**
```sql
SELECT * FROM TableName WHERE Field1 > '10' AND Field2 < '20';
SELECT * FROM TableName WHERE Field1 = 'value1' OR Field2 = 'value2';
SELECT * FROM TableName WHERE NOT Field1 = 'value';
SELECT * FROM TableName WHERE (Field1 > '10' AND Field2 < '20') OR Field3 = 'value';
```

**SELECT (LIKE, IN, BETWEEN):**
```sql
SELECT * FROM TableName WHERE FieldName LIKE 'prefix%';  -- Prefix match
SELECT * FROM TableName WHERE FieldName LIKE '%suffix';  -- Suffix match
SELECT * FROM TableName WHERE FieldName LIKE '%contains%';  -- Contains match
SELECT * FROM TableName WHERE FieldName IN ('value1', 'value2', 'value3');
SELECT * FROM TableName WHERE FieldName BETWEEN 'value1' AND 'value2';  -- Inclusive
```

**SELECT (GROUP BY and Aggregate Functions):**
```sql
SELECT COUNT(*) FROM TableName;
SELECT SUM(FieldName) FROM TableName;
SELECT AVG(FieldName) FROM TableName;
SELECT MAX(FieldName) FROM TableName;
SELECT MIN(FieldName) FROM TableName;
SELECT FieldName, COUNT(*) FROM TableName GROUP BY FieldName;
SELECT FieldName, COUNT(*), SUM(OtherField) FROM TableName GROUP BY FieldName;
```

**SELECT (UNION):**
```sql
SELECT Field1, Field2 FROM Table1 UNION SELECT Field1, Field2 FROM Table2;  -- Remove duplicates
SELECT Field1, Field2 FROM Table1 UNION ALL SELECT Field1, Field2 FROM Table2;  -- Keep duplicates
SELECT Field1 FROM Table1 UNION SELECT Field1 FROM Table2 UNION SELECT Field1 FROM Table3;  -- Multiple UNIONs
SELECT Field1, Field2 FROM Table1 UNION SELECT Field1, Field2 FROM Table2 ORDER BY Field1;  -- ORDER BY applies to final result
```

**SELECT (Subqueries):**
```sql
-- Scalar subquery
SELECT * FROM Table1 WHERE Field1 = (SELECT Field1 FROM Table2 WHERE Field2 = 'value');
SELECT * FROM Table1 WHERE Field1 > (SELECT AVG(Field1) FROM Table1);

-- IN subquery
SELECT * FROM Table1 WHERE Field1 IN (SELECT Field1 FROM Table2);

-- EXISTS subquery
SELECT * FROM Table1 WHERE EXISTS (SELECT * FROM Table2 WHERE Table2.Field1 = Table1.Field1);
SELECT * FROM Table1 WHERE NOT EXISTS (SELECT * FROM Table2 WHERE Table2.Field1 = Table1.Field1);

-- Nested subquery
SELECT * FROM Table1 WHERE Field1 = (SELECT Field1 FROM Table2 WHERE Field2 = (SELECT MAX(Field2) FROM Table2));
```

**SELECT (HAVING):**
```sql
SELECT FieldName, COUNT(*) FROM TableName GROUP BY FieldName HAVING COUNT(*) > 1;
SELECT FieldName, COUNT(*) FROM TableName GROUP BY FieldName HAVING FieldName > '25';
SELECT FieldName, COUNT(*) FROM TableName GROUP BY FieldName HAVING COUNT(*) > 1 AND FieldName < '30';
```

**Note**: 
- The system supports INNER JOIN, LEFT JOIN, RIGHT JOIN, FULL OUTER JOIN, and NATURAL JOIN (with all variants).
- The system supports ORDER BY (single or multiple fields, ASC/DESC), DISTINCT, LIMIT, comparison operators (>, <, >=, <=, !=), and complex WHERE conditions (AND, OR, NOT with parentheses support).
- LIKE pattern matching (case-sensitive, supports % wildcard), IN clause (with value lists and subqueries), and BETWEEN range queries are fully implemented.
- GROUP BY grouping, aggregate functions (COUNT, SUM, AVG, MAX, MIN), and HAVING clause are fully implemented.
- UNION and UNION ALL are fully implemented, supporting multiple UNION connections and global ORDER BY/LIMIT.
- Subqueries are fully implemented, including scalar subqueries, IN subqueries, EXISTS/NOT EXISTS subqueries, correlated subqueries, and nested subqueries.

For detailed SQL execution test cases, please refer to [docs/testing/sql_execution_test_cases.md](../docs/testing/sql_execution_test_cases.md).

---

## Index Management

### Creating an Index

1. Navigate to the **Index Management** tab
2. Select a table
3. Select a field
4. Choose index type:
   - **Adjacent Index**: For range queries and sequential scans
   - **Hash Index**: For equality queries (primary keys)
5. Click **Create Index**

### Viewing Indexes

1. Navigate to the **Index Management** tab
2. View all indexes in the index list
3. See index statistics (type, field, performance metrics)

### Index Recommendations

The system automatically analyzes query logs and provides index recommendations:

1. Navigate to the **Index Management** tab
2. Click **View Recommendations**
3. Review recommended indexes with:
   - Expected performance improvement
   - Recommendation reason
   - Usage statistics
4. Click **Create Recommended Index** to apply

### Deleting an Index

1. Navigate to the **Index Management** tab
2. Select an index
3. Click **Delete Index**
4. Confirm deletion

---

## Keyboard Shortcuts

### General Shortcuts

- **Ctrl+Q** or **Alt+F4**: Exit application
- **F1**: Show help / About dialog
- **Ctrl+Tab**: Switch between tabs (next)
- **Ctrl+Shift+Tab**: Switch between tabs (previous)

### Table Management

- **Ctrl+N**: Create new table (when implemented)
- **Ctrl+E**: Edit selected table (when implemented)
- **Delete**: Delete selected table (when implemented)

### Data Operation

- **Ctrl+I**: Insert new record (when implemented)
- **Ctrl+S**: Save changes (when implemented)
- **Delete**: Delete selected record (when implemented)

### SQL Execution

- **Ctrl+Enter**: Execute SQL statement
- **Ctrl+R**: Execute SQL statement (alternative)
- **Ctrl+L**: Clear SQL editor (when implemented)
- **Ctrl+Up/Down**: Navigate SQL history (when implemented)

### Index Management

- **Ctrl+I**: Create new index (when implemented)
- **F5**: Refresh index list (when implemented)

---

## Troubleshooting

### Common Issues

#### Issue: "Cannot open database file"

**Solution:**
- Ensure the database file path is correct
- Check file permissions
- Verify the database file exists

#### Issue: "Table not found"

**Solution:**
- Verify the table name is correct (case-sensitive)
- Check if the table exists in the database
- Ensure the database file is loaded

#### Issue: "SQL syntax error"

**Solution:**
- Check SQL syntax against the supported syntax guide
- Verify all keywords are spelled correctly
- Ensure proper use of IN/INTO clauses

#### Issue: "Field type mismatch"

**Solution:**
- Verify field types match the table definition
- Check data format (e.g., numbers vs strings)
- Ensure NULL values are allowed if used

### Getting Help

- Check the **Guide** tab for detailed information
- Review SQL syntax examples in this guide
- Check error messages in the status bar

---

## Tips and Best Practices

1. **Use Indexes Wisely**: Create indexes on frequently queried fields
2. **Backup Data**: Regularly backup `.dbf` and `.dat` files
3. **Use Transactions**: When implemented, use transactions for multiple operations
4. **Optimize Queries**: Use WHERE clauses to limit result sets
5. **Monitor Performance**: Check index recommendations regularly

---

## Version Information

- **Current Version**: v0.7.1
- **Last Updated**: 2026-01-15

### Recent Updates (v0.7.1)

- ✅ NATURAL JOIN implementation (NATURAL JOIN, NATURAL LEFT/RIGHT/INNER/FULL JOIN)
- ✅ UNION and UNION ALL implementation (with multiple UNION connections and global ORDER BY/LIMIT)
- ✅ Subquery implementation (scalar, IN, EXISTS/NOT EXISTS, correlated, nested subqueries)
- ✅ Fixed floating-point comparison precision issue (using numeric comparison instead of string comparison)

### Previous Updates (v0.7.0)

- ✅ SQL Execution interface completed
- ✅ Batch SQL execution support
- ✅ Primary key uniqueness constraint enforcement
- ✅ Case-insensitive table names and keywords
- ✅ JOIN query support (INNER, LEFT, RIGHT, FULL OUTER JOIN)
- ✅ ORDER BY, DISTINCT, LIMIT support
- ✅ Comparison operators (>, <, >=, <=, !=)
- ✅ Complex WHERE conditions (AND, OR, NOT with parentheses)
- ✅ LIKE pattern matching, IN clause, BETWEEN range queries
- ✅ GROUP BY grouping and aggregate functions (COUNT, SUM, AVG, MAX, MIN)
- ✅ HAVING clause for filtering grouped results
- ✅ Fixed DROP TABLE data deletion issue
- ✅ Fixed RENAME TABLE file synchronization issue
- ✅ Comprehensive SQL test cases added

---

**Note**: This guide will be updated as new features are added to the system.

