
/*
    Storage -

    Discription:    Storage is an abstruct class.
                    It is an interface for manipulating memroy on a general
					storage unit

    Date:           27.1.2020

    Ver 1.0
*/

#ifndef __HRD11_STORAGE_HPP__
#define __HRD11_STORAGE_HPP__

#include <memory>		// std::unique_ptr

#include "driver_data.hpp"

namespace hrd11
{

class Storage
{
public:
	// uncopiable
	Storage() = default;
	virtual ~Storage() = default;

	virtual std::unique_ptr<DriverData> Write(std::unique_ptr<DriverData> data) = 0;
	virtual std::unique_ptr<DriverData> Read(std::unique_ptr<DriverData> data) = 0;
};

}	// namespace hrd11

#endif // __HRD11_STORAGE_HPP__
