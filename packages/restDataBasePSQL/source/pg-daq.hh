#ifndef PG_DAQ_H_
#define PG_DAQ_H_

#include <string>

namespace pg {

  unsigned int get_new_run ();

  std::string add_new_file(unsigned int run_idx, unsigned int file_idx);

  void update_file_info_when_close (unsigned int run_idx, unsigned int file_idx, const std::string & file_name, unsigned int file_size);
}

#endif // PG_DAQ_H_
