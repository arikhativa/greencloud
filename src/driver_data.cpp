
#include "driver_data.hpp"

namespace hrd11
{

DriverData::DriverData()
{}

DriverData::DriverData(unsigned int len) : m_len(len), m_buff(len)
{}

}   // end namespace hrd11
