//////////////////////////////////////////////////////////////////////////
// godefarig, (kind of) spherical terrain generator                     //
// Copyright (C) 2016-2017 mooss <mooss@protonmail.com>                 //
// This file is part of godefarig.                                      //
//                                                                      //
// godefarig is free software: you can redistribute it and/or modify    //
// it under the terms of the GNU General Public License as published by //
// the Free Software Foundation, either version 3 of the License, or    //
// (at your option) any later version.                                  //
//                                                                      //
// godefarig is distributed in the hope that it will be useful,         //
// but WITHOUT ANY WARRANTY; without even the implied warranty of       //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        //
// GNU General Public License for more details.                         //
//                                                                      //
// You should have received a copy of the GNU General Public License    //
// along with godefarig.  If not, see <http://www.gnu.org/licenses/>.   //
//////////////////////////////////////////////////////////////////////////

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

//###################################################
//################# simple chrono ###################
//###                                             ###

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

//###################################################
//################### delta time ####################
//###                                             ###

void gfg::delta_time::reset()
{
    delta_ = 0;
    last_ = std::chrono::system_clock::now();
}

void gfg::delta_time::update()
{
    auto now_tmp = std::chrono::system_clock::now();
    delta_ = std::chrono::duration<double>(now_tmp - last_).count();
    last_ = now_tmp;
}

//###################################################
//############## parameter exception ################
//###                                             ###

std::invalid_argument gfg::parameter_exception(const std::string& option, const std::vector<std::string>& valid_values, const std::string& got)
{
    std::string msg = "\ninvalid option: " + option + " got value: `" + got + "` instead of the expected";
    for(auto& el : valid_values)
        msg += "\n\t`" + el + "`";
    return std::invalid_argument(msg);
}
