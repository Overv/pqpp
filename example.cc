#include "pq.hpp"
#include <iostream>

int main() {
    auto db = pq::connection("host=127.0.0.1 dbname=test user=test password=test");

    db.exec("CREATE TABLE people (name TEXT, age INTEGER)");

    db.exec("INSERT INTO people VALUES ($1, $2)", {"John", "26"});
    db.exec("INSERT INTO people VALUES ($1, $2)", {"Fred", "53"});
    db.exec("INSERT INTO people VALUES ($1, $2)", {"Lisa", "37"});

    auto rows = db.exec("SELECT * FROM people ORDER BY age ASC");

    for (auto& row : rows) {
        int age = row["age"];
        std::string name = row["name"];

        std::cout << name << " is " << age << " years old" << std::endl;
    }

    db.exec("DROP TABLE people");

    return 0;
}
