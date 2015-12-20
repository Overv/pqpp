#ifndef PQ_HPP
#define PQ_HPP

#include <postgresql/libpq-fe.h>

#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <map>

namespace pq {
    using std::string;
    using std::vector;
    using std::shared_ptr;
    using std::unique_ptr;
    using std::runtime_error;
    using std::function;
    using std::map;
    using std::to_string;
    using std::nullptr_t;

    // Container for PostgreSQL row values that allows for easy conversions
    class value {
    public:
        value() {}
        value(const string& val, bool null) : val(val), null(null) {}

        bool is_null() const {
            return null;
        }

        template<typename T> T get() const {
            return static_cast<T>(val);
        }

        template <typename T> operator T() const {
            return get<T>();
        }

    private:
        string val;
        bool null;
    };

    template<> int value::get<int>() const {
        return std::stoi(val);
    }

    template<> long value::get<long>() const {
        return std::stol(val);
    }

    template<> long long value::get<long long>() const {
        return std::stoll(val);
    }

    template<> unsigned long value::get<unsigned long>() const {
        return std::stoul(val);
    }

    template<> unsigned long long value::get<unsigned long long>() const {
        return std::stoull(val);
    }

    template<> double value::get<double>() const {
        return std::stod(val);
    }

    template<> long double value::get<long double>() const {
        return std::stold(val);
    }

    template<> float value::get<float>() const {
        return std::stof(val);
    }

    template<> bool value::get<bool>() const {
        return val != "false" && val != "";
    }

    // Helper for parameterized queries
    void _make_value_list(vector<value>& list) {}

    template<typename T, typename... Args>
    void _make_value_list(vector<value>& list, T n, Args... rest) {
        list.push_back(value(to_string(n), false));
        _make_value_list(list, rest...);
    }

    template<typename... Args>
    void _make_value_list(vector<value>& list, const char* str, Args... rest) {
        list.push_back(value(str, false));
        _make_value_list(list, rest...);
    }

    template<typename... Args>
    void _make_value_list(vector<value>& list, nullptr_t null, Args... rest) {
        list.push_back(value("", true));
        _make_value_list(list, rest...);
    }

    template<typename... Args>
    vector<value> _make_value_list(Args... rest) {
        vector<value> list;
        _make_value_list(list, rest...);
        return list;
    }

    // Row is represented as hash table with column names
    typedef map<string, value> row_t;

    class connection {
    public:
        connection(const string& connInfo) {
            conn = shared_ptr<PGconn>(PQconnectdb(connInfo.c_str()), [=](PGconn* conn) {
                PQfinish(conn);
            });

            if (PQstatus(conn.get()) != CONNECTION_OK) {
                throw runtime_error(PQerrorMessage(conn.get()));
            }
        }

        template<typename... Args>
        vector<row_t> exec(const string& query, Args... param_args) {
            vector<value> params = _make_value_list(param_args...);
            vector<const char*> values;

            for (auto& val : params) {
                if (val.is_null()) {
                    values.push_back(nullptr);
                } else {
                    values.push_back(val.get<string>().c_str());
                }
            }

            auto tmp = PQexecParams(conn.get(), query.c_str(), values.size(), nullptr, values.data(), nullptr, nullptr, 0);
            auto res = unique_ptr<PGresult, function<void(PGresult*)>>(tmp, [=](PGresult* res) {
                PQclear(res);
            });

            int status = PQresultStatus(res.get());

            if (status == PGRES_COMMAND_OK) {
                return vector<row_t>();
            } else if (status == PGRES_TUPLES_OK) {
                // Determine columns in result
                map<string, int> columnIds;

                int columnCount = PQnfields(res.get());
                for (int c = 0; c < columnCount; c++) {
                    columnIds.emplace(PQfname(res.get(), c), c);
                }

                // Extract rows
                vector<row_t> rows;

                int rowCount = PQntuples(res.get());
                for (int r = 0; r < rowCount; r++) {
                    row_t row;

                    for (auto& column : columnIds) {
                        string val = PQgetvalue(res.get(), r, column.second);
                        bool isNull = PQgetisnull(res.get(), r, column.second);

                        row.emplace(column.first, value(val, isNull));
                    }

                    rows.push_back(row);
                }

                return rows;
            } else {
                throw runtime_error(PQerrorMessage(conn.get()));
            }
        }

    private:
        shared_ptr<PGconn> conn;
    };
}

#endif
