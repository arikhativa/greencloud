
#ifndef __HRD11_TP_SYS_TASK_HPP__
#define __HRD11_TP_SYS_TASK_HPP__

#include "tptask.hpp"

namespace hrd11
{

struct ThreadMsg
{
    enum MsgType
    {
        STOP = 0,
        SUSPEND,
        RESUME
    };
        ThreadMsg(MsgType msg) : m_msg(msg)
        {}

        MsgType m_msg;
};

struct TPStop : public TPTask
{
	TPStop() : TPTask(TPTask::TaskPriority::SYSTEM)
	{}
	~TPStop()
	{}

	void Execute() override
	{
		throw ThreadMsg(ThreadMsg::MsgType::STOP);
	}
};

struct TPSuspend : public TPTask
{
	TPSuspend() : TPTask(TPTask::TaskPriority::SYSTEM)
	{}
	~TPSuspend()
	{}

	void Execute() override
	{
		throw ThreadMsg(ThreadMsg::MsgType::SUSPEND);
	}
};

}	// end namespace hrd11

#endif // __HRD11_TP_SYS_TASK_HPP__
