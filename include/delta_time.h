/**
 * @file
 * @brief class dealing with time
 * @date 11 february 2017
 * @author mooss
 */
#ifndef MOOSS_DELTA_TIME_H
#define MOOSS_DELTA_TIME_H

#include <chrono>

namespace gfg{

/** @class delta_time
 * \ref simple class handling time deltas
 */
class delta_time
{
  public:
    delta_time() {}///< leaves everything to default
    void reset();///< reset the delta and restart the chrono
    void update();///< saves the delta and restart the chrono
    double delta() const { return delta_; }///< delta getter
  private:
    std::chrono::time_point<std::chrono::system_clock> last_;
    double delta_;
};//class delta_time
}//namespace gfg

#endif//MOOSS_DELTA_TIME_H
