/**
 * @file
 * @brief collection of various small classes
 * @date 11 february 2017
 * @author mooss
 */

#ifndef MOOSS_GFG_UTILS_H
#define MOOSS_GFG_UTILS_H
#include "glm/glm.hpp"
#include <chrono>
#include <stdexcept>
#include <memory>
#include <vector>

const double PI = 3.141592653589793;

namespace gfg
{
using index = unsigned int;
using composante = float;

using position = glm::tvec3<composante>;
using color = glm::tvec3<composante>;
using normal = glm::tvec3<composante>;
using elevation = composante;

namespace face
{
index numberAtStage(unsigned int);
};

namespace edge
{
index numberAtStage(unsigned int);
};

namespace point
{
index numberAtStage(unsigned int);
};

/** @class simple_chrono
 * \ref minimalistic helper class providing a small layer of abstraction on top of std::chrono
*/
class simple_chrono
{
  public:
    simple_chrono() {}///< leaves everything to default
    void start();///< fixes a new starting point
    void stop();///< fixes a new ending point
    double elapsed_time() const;///< @return the amount of time elapsed
  private:
    std::chrono::time_point<std::chrono::system_clock> m_start;
    std::chrono::time_point<std::chrono::system_clock> m_end;
    
};//simple chrono

/** @class delta_time
 * \ref helper class handling time deltas
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

/** generate a nicely formatted exception to handle incorrect arguments
 * @param option the option which was refused
 * @param valid_values the list of accepted values for this option
 * @param got the incorrect value received by this option
 * @return an std::invalid_argument describing the situation
 */
std::invalid_argument parameter_exception(const std::string& option, const std::vector<std::string>& valid_values, const std::string& got);

};//namespace gfg


#endif//MOOSS_GFG_UTILS_H
