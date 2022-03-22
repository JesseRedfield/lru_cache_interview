#include "IDiskCache.h"
#include "sqlite3.h"
#include <string>

#ifndef SQLITE_DISK_CACHE
#define SQLITE_DISK_CACHE

class SqliteDiskCache : public IDiskCache
{
public:
    SqliteDiskCache()
    {
    }

    ~SqliteDiskCache()
    {
        sqlite3_close(mpDB);
    }

    virtual bool initialize(const char *path)
    {
        int rc;
        char *zErrMsg = 0;

        // Open or Create Database
        rc = sqlite3_open(path, &mpDB);

        if (rc)
        {
            fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(mpDB));
            return false;
        }

        bool bExists = false;
        // Exit Early if Cache Table is already created
        sqlite3_stmt *pSelectStatement = NULL;
        rc = sqlite3_prepare(mpDB, "SELECT name FROM sqlite_master WHERE type='table' AND name='CACHE';", -1, &pSelectStatement, 0);
        if ((rc == SQLITE_OK) && (pSelectStatement != NULL))
        {
            rc = sqlite3_step(pSelectStatement);

            if (rc == SQLITE_ROW)
            {
                sqlite3_clear_bindings(pSelectStatement);
                sqlite3_reset(pSelectStatement);
                bExists = true;
            }

            rc = sqlite3_finalize(pSelectStatement);
        }

        if (bExists)
        {
            // clear cache and return true if table exists
            rc = sqlite3_exec(mpDB, "DELETE FROM CACHE;", callback, 0, &zErrMsg);
            if (rc != SQLITE_OK)
            {
                fprintf(stderr, "SQL ERROR: %s\n", zErrMsg);
                sqlite3_free(zErrMsg);
                return false;
            }

            return true;
        }

        // Create Table for disk cache
        rc = sqlite3_exec(mpDB, "CREATE TABLE CACHE (ID TEXT, VALUE TEXT);", callback, 0, &zErrMsg);
        if (rc != SQLITE_OK)
        {
            fprintf(stderr, "SQL ERROR: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
            return false;
        }

        return true;
    }

    virtual void put(LRUCacheItem *pItem)
    {
        if (updateRow(pItem))
            return;

        addRow(pItem);
    }

    virtual LRUCacheItem *get(std::string identifier)
    {
        int rc;
        char *zErrMsg = 0;

        std::string statement = "SELECT * FROM CACHE WHERE ID = '";
        statement.append(identifier);
        statement.append("';");
        char cStmt[statement.length() + 1];
        strcpy(cStmt, statement.c_str());

        sqlite3_stmt *pSelectStatement = NULL;
        rc = sqlite3_prepare(mpDB, cStmt, -1, &pSelectStatement, 0);

        if ((rc == SQLITE_OK) && (pSelectStatement != NULL))
        {

            rc = sqlite3_step(pSelectStatement);

            if (rc == SQLITE_ROW)
            {
                std::string key((char *)sqlite3_column_text(pSelectStatement, 0));
                std::string value((char *)sqlite3_column_text(pSelectStatement, 1));

                LRUCacheItem *pItem = new LRUCacheItem(key);
                pItem->deserialize(value);

                sqlite3_clear_bindings(pSelectStatement);
                sqlite3_reset(pSelectStatement);
                rc = sqlite3_finalize(pSelectStatement);

                return pItem;
            }

            rc = sqlite3_finalize(pSelectStatement);
        }

        return NULL;
    }

private:
    sqlite3 *mpDB;

    static int callback(void *NotUsed, int argc, char **argv, char **azColName)
    {
        return 0;
    }

    bool updateRow(LRUCacheItem *pItem)
    {
        int rc;
        char *zErrMsg = 0;

        // Update Row If It Already Exists
        std::string statement = "UPDATE CACHE SET VALUE='";
        statement.append(pItem->serialize());
        statement.append("' WHERE ID = '");
        statement.append(pItem->get_cache_id());
        statement.append("';");
        char cStmt[statement.length() + 1];
        strcpy(cStmt, statement.c_str());

        rc = sqlite3_exec(mpDB, cStmt, callback, 0, &zErrMsg);
        if (rc == SQLITE_OK && sqlite3_changes(mpDB) > 0)
        {
            return true;
        }
        else if (rc == SQLITE_OK)
        {
            return false;
        }
        else
        {
            fprintf(stderr, "SQL ERROR: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
            return false;
        }
    }

    bool addRow(LRUCacheItem *pItem)
    {
        int rc;
        char *zErrMsg = 0;

        // Update Row If It Already Exists
        std::string statement = "INSERT INTO CACHE (ID, VALUE) VALUES('";
        statement.append(pItem->get_cache_id());
        statement.append("','");
        statement.append(pItem->serialize());
        statement.append("');");
        char cStmt[statement.length() + 1];
        strcpy(cStmt, statement.c_str());

        rc = sqlite3_exec(mpDB, cStmt, callback, 0, &zErrMsg);
        if (rc == SQLITE_OK)
        {
            return true;
        }
        else
        {
            fprintf(stderr, "SQL ERROR: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
            return false;
        }
    }
};

#endif