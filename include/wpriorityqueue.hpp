
/*
    WPriorityQueue -

    Discription:API for W_pq.
    Wrapper class for thread safe  & waitable priority queue.
    Base on std::priority_queue.

    Requierment:    Is before function. (lhs before rhs).

    Exception:      bad_alloc
                    SemError

    Date: 30.1.20
    Ver 1.0
*/

#ifndef __W_PQ_HPP_
#define __W_PQ_HPP_

#include <queue>
#include <mutex>
#include <vector>
#include <chrono>

#include "semaphore.hpp"

namespace hrd11
{

template<
typename T,
typename Container = std::vector<T>,
typename Compare = std::less<typename Container::value_type>>
class WPriorityQueue
{
public:
    // uncopiable
    WPriorityQueue() = default;
    ~WPriorityQueue() = default;

    WPriorityQueue(const WPriorityQueue&) = delete;
    WPriorityQueue& operator=(const WPriorityQueue&) = delete;

    WPriorityQueue(WPriorityQueue&&) = delete;
    WPriorityQueue& operator=(WPriorityQueue&&) = delete;

    void Pop(T& ret_val);
    bool Pop(T& ret_val, std::chrono::seconds timeout);

    void Push(const T& new_val);
    void Push(T&& new_val);

    size_t Size() const;
    bool IsEmpty() const;

private:
    void LockAndPop(T& new_val);

    Semaphore m_sem;
    std::mutex m_mutex;
    std::priority_queue<T, Container, Compare> m_pq;
};


// Public Members -------------------------------------------------------------

template<typename T, typename Container, typename Compare>
void WPriorityQueue<T, Container, Compare>::Pop(T& ret_val)
{
    m_sem.Wait();

    LockAndPop(ret_val);
}

template<typename T, typename Container, typename Compare>
bool WPriorityQueue<T, Container, Compare>::Pop(T& ret_val,
                                                std::chrono::seconds timeout)
{
    try
    {
        m_sem.TimedWait(timeout.count());
    }
    catch (const SemTimeOut& e)
    {
        return false;
    }

    LockAndPop(ret_val);

    return true;
}

template<typename T, typename Container, typename Compare>
void WPriorityQueue<T, Container, Compare>::Push(const T& new_val)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_pq.push(new_val);
    m_sem.Post();
}

template<typename T, typename Container, typename Compare>
void WPriorityQueue<T, Container, Compare>::Push(T&& new_val)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_pq.push(std::move(new_val));
    m_sem.Post();
}

template<typename T, typename Container, typename Compare>
size_t WPriorityQueue<T, Container, Compare>::Size() const
{
    return m_pq.size();
}

template<typename T, typename Container, typename Compare>
bool WPriorityQueue<T, Container, Compare>::IsEmpty() const
{
    return m_pq.empty();
}


// Private Members ------------------------------------------------------------

template<typename T, typename Container, typename Compare>
void WPriorityQueue<T, Container, Compare>::LockAndPop(T& ret_val)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    ret_val = std::move(const_cast<T&>(m_pq.top()));
    m_pq.pop();
}

} // namespace hrd11

#endif //__W_PQ_HPP_
