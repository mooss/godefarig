#ifndef MOOSS_ASSERTION_EXCEPTION_H
#define MOOSS_ASSERTION_EXCEPTION_H

#include <exception>
#include <string>
namespace gfg{

class assertion_exception : public std::exception
{
  public:
    assertion_exception( const std::string& expression, const std::string& file, unsigned int line, const std::string& message);
    const char* what() const override throw();

  private:
    std::string expression_;
    std::string file_;
    unsigned int line_;
    std::string message_;
    std::string report_;
};//class assertion_exception

}//namespace gfg
#endif//MOOSS_ASSERTION_EXCEPTION_H
