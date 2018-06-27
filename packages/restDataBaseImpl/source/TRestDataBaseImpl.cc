#include "TRestDataBaseImpl.hh"

TRestDataBaseImpl::TRestDataBaseImpl() {
	conn = pg::connect("postgresql://p3daq:ilovepanda@localhost/p3_daq");
}

TRestDataBaseImpl::~TRestDataBaseImpl() {

}

int TRestDataBaseImpl::test();
{
	auto result = pg::query(conn, "select * from files", 1, 16, 1);
	for (size_t row = 0; row<result.size() && row<10; ++row) {
		cout << result[row][3] << endl;
	}
	return 0;
}


