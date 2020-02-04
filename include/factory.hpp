/*
    Factory -

    Discription:    Factory a design pattern for creating specific instance
                    among abstract family of classes. The special value is to enable
                    create concrete instance at run time.

    Usage:          Enter key and pointer to static creation function of any class.

    Requirement:    Any class at the inheritence family should supply
                    static creation function.
                    'Key' must be compareable by operator "==".

    Exceptions:     std::bad_alloc
                    Badkey<>

    Date:           29.1.2020
    Folder name:    greencloud/include
    File names:     factory.hpp

*/

#ifndef __HRD11_FACTORY_HPP__
#define __HRD11_FACTORY_HPP__

#include <functional>       // std::function
#include <unordered_map>    // std::unordered_map
#include <memory>           // std::unique_ptr
#include <stdexcept>        // std::logic_error

namespace hrd11
{

enum FactoryAddStatus
{
    NEW = 0,
    UPDATED,
};

template<typename Key>
struct Badkey : public std::logic_error
{
    explicit Badkey(const std::string& msg, const Key& bad_key) :
    std::logic_error(msg),
    m_bad_key(bad_key)
    {}

    virtual const char* what() const throw () override
    {
        return std::logic_error::what();
    }

    const Key& m_bad_key;
};


template <typename Base, typename Key, typename Args>
class Factory
{
public:

    Factory() = default;
    ~Factory() = default;

    Factory(const Factory& other) = delete;
    Factory& operator = (const Factory& other) = delete;
    Factory(Factory&& other) = delete;
    Factory& operator = (Factory&& other) = delete;

    using CreatorFunc = std::function<std::unique_ptr<Base>(Args args)>;
    FactoryAddStatus Add(const Key& key, CreatorFunc func);

    std::unique_ptr<Base> Create(const Key& key, Args args);

private:
    std::unordered_map<Key, CreatorFunc> m_umap;
};

template <typename Base, typename Key, typename Args>
FactoryAddStatus
Factory<Base, Key, Args>::Add(const Key& key, CreatorFunc func)
{
    FactoryAddStatus ret = NEW;

    if (m_umap.find(key) != m_umap.end())
    {
        ret = UPDATED;
    }

    m_umap[key] = func;

    return ret;
}

template <typename Base, typename Key, typename Args>
std::unique_ptr<Base>
Factory<Base, Key, Args>::Create(const Key& key, Args args)
{
    auto iter = m_umap.find(key);

    if (iter == m_umap.end())
    {
        throw Badkey<Key>("Factory<>::Create() - invalid key", key);
    }

    return std::unique_ptr<Base>(iter->second(args));
}

}   // end namespace hrd11

#endif //__HRD11_FACTORY_HPP__
