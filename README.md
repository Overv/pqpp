pqpp
====

This is a header only library with simple libpq bindings for modern C++. It
provides the following features:

* Memory safety (libpq objects are managed with `shared_ptr` and `unique_ptr`)
* Errors are propagated as `runtime_error` exceptions with clear descriptions
* Rows are returned as column -> value mappings that can be cast to common types

To do
-----

* Support for binary data (`BLOB`, `TEXT` with `\0` bytes)
* Wrapper for cursors

Example
-------

```c++
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
```

License
-------

Licensed using the MIT license.

    Copyright (c) 2015 Alexander Overvoorde

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
    IN THE SOFTWARE.
