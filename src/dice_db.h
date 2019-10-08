#pragma once
#include <memory>
#include "SQLiteCpp/SQLiteCpp.h"

namespace dice::db
{
    extern std::unique_ptr<SQLite::Database> db;
    void InitialiseDB();
}
