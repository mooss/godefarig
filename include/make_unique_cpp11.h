#ifndef MOOSS_MAKE_UNIQUE_CPP11_H
#define MOOSS_MAKE_UNIQUE_CPP11_H

#include <memory>

namespace std
{

template<typename T, typename ...Args>
std::unique_ptr<T> make_unique( Args&& ...args )
{
    return std::unique_ptr<T>( new T( std::forward<Args>(args)... ) );
}

}
#endif//MOOSS_MAKE_UNIQUE_CPP11_H
