#include "gfg_utils.h"
#include <cmath>//pow

gfg::index gfg::face::numberAtStage(unsigned int stageNum)
{
    return 8 * pow(4ul, stageNum);
}

gfg::index gfg::edge::numberAtStage(unsigned int stageNum)
{
    return 3 * pow(4ul, stageNum+1);
}

gfg::index gfg::point::numberAtStage(unsigned int stageNum)
{
    return 4 * pow(4ul, stageNum) + 2;
}

void gfg::simple_chrono::start()
{
    m_start = std::chrono::system_clock::now();
}

void gfg::simple_chrono::stop()
{
    m_end = std::chrono::system_clock::now();
}

double gfg::simple_chrono::elapsed_time() const
{
    return std::chrono::duration<double>(m_end - m_start).count();
}


std::invalid_argument gfg::parameter_exception(const std::string& option, const std::vector<std::string>& valid_values, const std::string& got)
{
    std::string msg = "\ninvalid option: " + option + " got value: `" + got + "` instead of the expected";
    for(auto& el : valid_values)
        msg += "\n\t`" + el + "`";
    return std::invalid_argument(msg);
}
