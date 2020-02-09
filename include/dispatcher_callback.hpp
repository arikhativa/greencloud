/*
    Dispatcher - CallBack :

    Based on "call_back / pub-sub" design pattern.
    It enable relationship between subject class and many dependencies objects.
    By two mediators you can send notification from subject to all dependencies that
    their behavior change corresponding to the subject.

    Requierment: Impemet "DoAction" method, that recive the massage.

    Date: 5.2.20
    Ver 1.0
*/

#ifndef __DISPATHCER_CALLBACK_HPP_
#define __DISPATHCER_CALLBACK_HPP_

#include <iosfwd>
#include <vector>       // std::vector
#include <algorithm>    // std::swap
#include <utility>      // std::swap

namespace hrd11
{

template<typename Msg>
class Dispatcher;

// CallBackBase ---------------------------------------------------------------
template<typename Msg>
class CallBackBase
{
public:
    explicit CallBackBase(Dispatcher<Msg>* dispatcher);
    virtual ~CallBackBase();

    CallBackBase(const CallBackBase&) = delete;
    CallBackBase& operator=(const CallBackBase&) = delete;

    CallBackBase(CallBackBase&&) = delete;
    CallBackBase& operator=(CallBackBase&&) = delete;

    virtual void OutOfService() = 0;
    // TODO: make UpdateDispatcher()
    // and then dont force ctor(* dispatcher)

private:
    friend class Dispatcher<Msg>;

    virtual void Notify(const Msg& massage) = 0;

    Dispatcher<Msg>* m_dispatcher;
};


// CallBack -------------------------------------------------------------------
template<typename Msg, typename Observer>
class CallBack : public CallBackBase<Msg>
{
public:
    using ActionMethod = void(Observer::*)(const Msg&);
    using StopMethod = void(Observer::*)();

    CallBack(Dispatcher<Msg>* dispatcher,
                Observer& observer,
                ActionMethod action_method,
                StopMethod stop_method = nullptr);
    ~CallBack() = default;

private:
    void Notify(const Msg& massage) override;
    void OutOfService() override;

    Observer& m_observer;
    ActionMethod m_action_method;
    StopMethod m_stop_method;
};


// Dispatcher -----------------------------------------------------------------
template<typename Msg>
class Dispatcher
{
public:
    Dispatcher() = default;
    ~Dispatcher(); // should notify out of service

    Dispatcher(const Dispatcher&) = delete;
    Dispatcher& operator=(const Dispatcher&) = delete;

    Dispatcher(Dispatcher&&) = delete;
    Dispatcher& operator=(Dispatcher&&) = delete;

    void NotifyAll(const Msg& massage);

private:
    friend class CallBackBase<Msg>;

    void Remove(CallBackBase<Msg>* callback);
    void Add(CallBackBase<Msg>* callback);

    std::vector<CallBackBase<Msg>*> m_callbacks;
};


// CallBackBase ---------------------------------------------------------------
template<typename Msg>
CallBackBase<Msg>::CallBackBase(Dispatcher<Msg>* dispatcher) :
        m_dispatcher(dispatcher)
{
    if (m_dispatcher)
    {
        m_dispatcher->Add(this);
    }
}

template<typename Msg>
CallBackBase<Msg>::~CallBackBase()
{
    if (m_dispatcher)
    {
        m_dispatcher->Remove(this);
    }
}

template<typename Msg>
void CallBackBase<Msg>::OutOfService()
{
    m_dispatcher = nullptr;
}


// CallBack -------------------------------------------------------------------
template<typename Msg, typename Observer>
CallBack<Msg, Observer>::CallBack(Dispatcher<Msg>* dispatcher,
                                    Observer& observer,
                                    ActionMethod action_method,
                                    StopMethod stop_method) :
        CallBackBase<Msg>(dispatcher),
        m_observer(observer),
        m_action_method(action_method),
        m_stop_method(stop_method)
{}

template<typename Msg, typename Observer>
void CallBack<Msg, Observer>::Notify(const Msg& massage)
{
    ((m_observer).*(m_action_method))(massage);
}

template<typename Msg, typename Observer>
void CallBack<Msg, Observer>::OutOfService()
{
    CallBackBase<Msg>::OutOfService();

    if (m_stop_method)
    {
        ((m_observer).*(m_stop_method))();
    }
}


// Dispatcher -----------------------------------------------------------------
template<typename Msg>
Dispatcher<Msg>::~Dispatcher()
{
    for(auto & iter : m_callbacks)
    {
            iter->OutOfService();
    }
}

template<typename Msg>
void Dispatcher<Msg>::NotifyAll(const Msg& massage)
{
    for(auto & iter : m_callbacks)
    {
        iter->Notify(massage);
    }
}

template<typename Msg>
void Dispatcher<Msg>::Remove(CallBackBase<Msg>* callback)
{
    auto iter = m_callbacks.begin();

    while (iter != m_callbacks.end())
    {
        if (*iter == callback)
        {
            std::swap(*iter, *(m_callbacks.end() - 1));
            m_callbacks.pop_back();

            break ;
        }
        ++iter;
    }
}

template<typename Msg>
void Dispatcher<Msg>::Add(CallBackBase<Msg>* callback)
{
    m_callbacks.push_back(callback);
}

} // end namespace hrd11

#endif //__DISPATHCER_CALLBACK_HPP_
