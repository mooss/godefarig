#include "utils/assertion_exception.h"

gfg::assertion_exception::assertion_exception( const std::string& expression, const std::string& file, unsigned int line, const std::string message):
    expression_(expression),
    file_(file),
    line_(line),
    message_(message)
{
    report_ = file_ + ":" + std::to_string(line_) + ": assertion '" + expression_ + "' failed : " + message_;
}

const char* gfg::assertion_exception::what() const throw()
{
    return report_.c_str();
}
