#include "pq.hpp"
#include <iostream>

int main() {
    auto db = pq::connection("host=127.0.0.1 dbname=test user=test password=test");

    db.exec("CREATE TABLE people (name TEXT, age INTEGER)");

    auto stmt = db.prepare("inserter", "INSERT INTO people VALUES ($1, $2)", 2);
    db.exec(stmt, "John", 26);
    db.exec(stmt, "Fred", 53);
    db.exec(stmt, "Lisa", 37);
    db.exec(stmt, nullptr, 18);

    auto rows = db.exec("SELECT * FROM people ORDER BY age ASC");

    for (auto& row : rows) {
        int age = row["age"];
        std::string name = row["name"];

        if (!row["name"].is_null()) {
            std::cout << name << " is " << age << " years old" << std::endl;
        }
    }

    db.exec("DROP TABLE people");

    return 0;
}
