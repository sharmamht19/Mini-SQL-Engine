#include "table.h"
/**
 * ========================
 * TABLE CATALOGUE CLASS
 * ========================
 *
 * @brief The TableCatalogue acts like an index of tables existing in the
 * system. Everytime a table is added(removed) to(from) the system, it needs to
 * be added(removed) to(from) the tableCatalogue.
 *
 * Manages the catalogue of tables in the system, offering functionalities to add, remove, and retrieve tables. This is essentially the metadata manager for all the tables stored in the database.
 *
 * ------------------------
 *
 * @brief insertTable:
 * Inserts a new table into the catalogue, using the table's name as the key to access it.
 *
 * ------------------------
 *
 * @brief deleteTable:
 * Deletes a specified table from the catalogue. This involves unloading the table data from memory, freeing up associated memory, and removing its entry from the tables map.
 *
 * ------------------------
 *
 * @brief getTable:
 * Retrieves a pointer to the table object with the specified name. If the table is present in the catalogue, its pointer is returned; otherwise, behavior is undefined.
 *
 * ------------------------
 *
 * @brief isTable:
 * Checks if a table with the specified name exists in the catalogue. Returns `true` if the table exists, `false` otherwise.
 *
 * ------------------------
 *
 * @brief isColumnFromTable:
 * Verifies whether a given column is a part of the specified table. Returns `true` if the column is present in the table, `false` otherwise.
 *
 * ------------------------
 *
 * @brief print:
 * Outputs a list of all tables (relations) present in the catalogue. This is useful for providing an overview of all the data tables available in the system.
 *
 * ------------------------
 *
 * @brief Destructor (TableCatalogue::~TableCatalogue):
 * Ensures proper cleanup when the catalogue object is destroyed. This involves unloading all tables from memory and freeing up their associated memory.
 *
 * ========================
 */

class TableCatalogue {
    unordered_map<string, Table*> tables;

   public:
    TableCatalogue() {}
    void insertTable(Table* table);
    void deleteTable(string tableName);
    Table* getTable(string tableName);
    bool isTable(string tableName);
    bool isColumnFromTable(string columnName, string tableName);
    void print();
    ~TableCatalogue();
};