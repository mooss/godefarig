#ifndef MOOSS_DELTA_TIME_H
#define MOOSS_DELTA_TIME_H

#include <chrono>

namespace gfg{

class delta_time
{
  public:
    delta_time() {}
    void reset();
    void update();
    double delta() { return delta_; }
  private:
    std::chrono::time_point<std::chrono::system_clock> last_;
    double delta_;
};//class delta_time
}//namespace gfg

#endif//MOOSS_DELTA_TIME_H
