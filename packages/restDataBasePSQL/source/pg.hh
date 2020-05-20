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
  int row;//row < 0 returns field names

  untyped_tuple(std::shared_ptr<PGresult> res, int i)
    : result_accessor(res), row(i)
  {}

  bool isnull(int col) const
  {
	if (row < 0) return false;
	return PQgetisnull(result.get(), row, col);
  }

  int size() const
  {
    return columns();
  }

  int columns() const
  {
    return PQnfields(result.get());
  }

  std::string operator[](int col) const
  {
	  if (row < 0) {
		  return std::string(PQfname(result.get(), col));
	  }
	  else
	  {
		  char* begin = PQgetvalue(result.get(), row, col);
		  char* end = begin + PQgetlength(result.get(), row, col);
		  return std::string(begin, end);
	  }
	  return "";
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

  int size() const
  {
    return rows();
  }

  int rows() const
  {
    return PQntuples(result.get());
  }

  int columns() const
  {
    return PQnfields(result.get());
  }

  untyped_tuple operator[](int i) const
  {
    return untyped_tuple(result, i);
  }
};

template <class ...ArgTypes>
untyped_result_set query(connection_t con, const std::string& text, ArgTypes ...args);

} // namespace pg

#include "pg-impl.hh"

extern std::shared_ptr<PGconn> conn;

#endif /* PG_H */
