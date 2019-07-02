#include <stdexcept>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include <iostream>
#include <sstream>
#include <utility>
#include <libpq-fe.h>
#include <pg-types.h>

namespace pg {

namespace impl {

struct casted_args_t {
  std::vector<std::string> vals;

  std::vector<const char*> values()
  {
    std::vector<const char*> c_strs(vals.size());
    std::transform(vals.begin(), vals.end(), c_strs.begin(), std::mem_fn(&std::string::c_str));
    return c_strs;
  }

  std::vector<int> lengths()
  {
    std::vector<int> lengths(vals.size());
    std::transform(vals.begin(), vals.end(), lengths.begin(), std::mem_fn(&std::string::size));
    return lengths;
  }
};

template <class T>
std::string cast_to_string(T v)
{
  std::ostringstream stream;
  stream << v;
  return stream.str();
}

template <class ...ArgTypes>
casted_args_t cast(ArgTypes ...args)
{
  return casted_args_t { std::vector<std::string> { cast_to_string(args)... } };
}

#include "pg-impl-type-oids.hh"

template <class ...ArgTypes>
std::vector<Oid> type_oids(ArgTypes...)
{
  return std::move(std::vector<Oid> { type_oid<ArgTypes>()... });
}

} // namespace impl

std::shared_ptr<PGconn> connect(std::string conninfo);

template <class ...ArgTypes>
untyped_result_set query(connection_t con, const std::string& text, ArgTypes ...args)
{
  using namespace impl;
  auto casted_args = cast(args...);
  auto result = std::shared_ptr<PGresult>(PQexecParams(
    con.get(), text.c_str(), sizeof...(args),
    type_oids(args...).data(),
    casted_args.values().data(), casted_args.lengths().data(),
    std::vector<int>(sizeof...(args), 0).data(),
    0), PQclear);
  auto status = PQresultStatus(result.get());
  if (status == PGRES_NONFATAL_ERROR)
    std::cerr << PQresultErrorMessage(result.get()) << std::endl;
  if (status == PGRES_FATAL_ERROR ||
      status == PGRES_BAD_RESPONSE) {
    throw std::runtime_error(PQresultErrorMessage(result.get()));
  }
  return untyped_result_set(result);
}

} // namespace pg
