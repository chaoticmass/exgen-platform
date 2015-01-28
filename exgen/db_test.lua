#!/bin/exgen
include("lib/serialize.lua");

db = db_open("test.db");
retval = db_exec(db, "CREATE TABLE IF NOT EXISTS testing(id INTEGER PRIMARY KEY, name TEXT);");
retval = db_exec(db, "INSERT INTO testing(name) VALUES('David');");
retval = db_exec(db, "SELECT * FROM testing;");

for i = 1, db_results.rows, 1 do
    for k, v in pairs(db_results[i]) do
	echo(k.." = "..v.."\n");
    end
    echo("\n");
end

--retval = db_exec(db, "SELECT name FROM testing WHERE id=5;");
retval = db_exec(db, "SELECT * FROM sqlite_master");

for i = 1, db_results.rows, 1 do
    for k, v in pairs(db_results[i]) do
	echo(k.." = "..v.."\n");
    end
    echo("\n");
end

echo(db_serialize(reference));
db_close(db);
