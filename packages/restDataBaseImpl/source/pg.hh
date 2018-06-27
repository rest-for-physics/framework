#ifndef PG_H
#define PG_H

#include <string>

#include <libpq-fe.h>

#include <memory>

namespace pg {

typedef std::shared_ptr<PGconn> connection_t;

std::shared_ptr<PGconn> connect(std::string conninfo);

struct result_accessor {
  std::shared_ptr<PGresult> result;

  result_accessor(std::shared_ptr<PGresult> res)
    : result(res)
  {}
};

struct untyped_tuple : result_accessor {
  size_t row;

  untyped_tuple(std::shared_ptr<PGresult> res, size_t i)
    : result_accessor(res), row(i)
  {}

  bool isnull(size_t col) const
  {
    return PQgetisnull(result.get(), row, col);
  }

  size_t size() const
  {
    return columns();
  }

  size_t columns() const
  {
    return PQnfields(result.get());
  }

  std::string operator[](size_t col) const
  {
    char* begin = PQgetvalue(result.get(), row, col);
    char* end = begin + PQgetlength(result.get(), row, col);
    return std::string(begin, end);
  }

  std::string operator[](std::string name) const
  {
    return (*this)[PQfnumber(result.get(), name.c_str())];
  }
};

struct untyped_result_set : result_accessor {
  untyped_result_set(std::shared_ptr<PGresult> res)
    : result_accessor(res)
  {}

  size_t size() const
  {
    return rows();
  }

  size_t rows() const
  {
    return PQntuples(result.get());
  }

  size_t columns() const
  {
    return PQnfields(result.get());
  }

  untyped_tuple operator[](size_t i) const
  {
    return untyped_tuple(result, i);
  }
};

template <class ...ArgTypes>
untyped_result_set query(connection_t con, const std::string& text, ArgTypes ...args);

} // namespace pg

#include "pg-impl.hh"

#endif /* PG_H */
