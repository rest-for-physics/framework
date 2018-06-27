#include "pg-daq.hh"
#include "pg.hh"

#include <sstream>
#include <iostream>
using std::istringstream;
using std::string;
using std::cerr;
using std::endl;

namespace pg {

  unsigned int get_new_run ()
  {
    int new_run_number;
    auto conn = connect("postgresql://p3daq:ilovepanda@localhost/p3_daq");
    auto result = query (conn,
			 "select new_run()");
    istringstream(result[0][0]) >> new_run_number;
    return new_run_number;
  }

  string add_new_file (unsigned int run_idx, unsigned int file_idx)
  {
    auto conn = connect("postgresql://p3daq:ilovepanda@localhost/p3_daq");
    auto result = query (conn, 
			 "insert into files (run_id, file_id) values ($1::bigint, $2::bigint) returning to_char(start_time, 'IYYYMMDD_HH24MISS_MS')",
			 run_idx, file_idx);
    string start_time;
    istringstream(result[0][0]) >> start_time;
    return start_time;
  }

    void update_file_info_when_close (unsigned int run_idx, unsigned int file_idx, const std::string & file_name, unsigned int file_size)
  {
    auto conn = connect("postgresql://p3daq:ilovepanda@localhost/p3_daq");
    try {
      query (conn,
	     "update files set stop_time = now(), file_name = $1, file_size = $2 where run_id = $3 and file_id = $4",
	     file_name.c_str(),
	     file_size,
	     run_idx,
	     file_idx);
    } catch (std::exception &e) {
      cerr << e.what() << endl;
    }
  }
}
