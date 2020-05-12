#include "pg.hh"

namespace pg {

std::shared_ptr<PGconn> connect(std::string conninfo) {
    auto con = std::shared_ptr<PGconn>(PQconnectdb(conninfo.c_str()), PQfinish);
    if (PQstatus(con.get()) != CONNECTION_OK) throw std::runtime_error(PQerrorMessage(con.get()));
    return con;
}

}  // namespace pg
