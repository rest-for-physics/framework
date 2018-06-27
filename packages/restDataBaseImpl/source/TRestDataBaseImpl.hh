#include <iostream>
#include "TRestDataBase.h"
#include "pg.hh"
using namespace std;


class TRestDataBaseImpl:public TRestDataBase {
public:
	TRestDataBaseImpl();
	~TRestDataBaseImpl();

	int test();

	
protected:
	std::shared_ptr<PGconn> conn;
};




