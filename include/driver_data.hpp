

/*
    DriverData -

    Discription:    DriverData is a class that represent the data that will go
					from socket to socket.

    Date:           27.1.2020

    Ver 1.0
*/

#ifndef __HRD11_DRIVER_DATA_HPP__
#define __HRD11_DRIVER_DATA_HPP__

#include <vector>	// vector()
#include <cstddef>	// size_t

namespace hrd11
{

static const int HANDLE_SIZE = 8;

enum DataType
{
	READ = 0,
	WRITE,
	DISCONNECT,
	FLUSH,
	TRIM,
	BAD_REQUEST
};

struct DriverData
{
	DriverData();
	DriverData(unsigned int len);

	void SetInfo(size_t offset, char handler[HANDLE_SIZE], DataType type);

	size_t m_offset;
	char m_handler[HANDLE_SIZE];
	unsigned int m_len;
	DataType m_type;
	int m_status; // describe expected status
	std::vector<char> m_buff;
};

}   // end namespace hrd11

#endif // __HRD11_DRIVER_DATA_HPP__
