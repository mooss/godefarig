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

#ifndef MOOSS_GLTOGGLE_H
#define MOOSS_GLTOGGLE_H

#include <string>
#include <functional>
#include <unordered_map>
#include "container_overload.h"
#include "myglad.h"//for GLenum, ...
//#include <algorithm>

namespace gfg{
namespace input{

class toggler
{
  public:
    toggler() {}
    virtual void callDefault()=0;
    virtual void callNext()=0;
    
    static std::shared_ptr<toggler> create(std::function<void(GLenum)>, std::vector<GLenum>);
    static std::shared_ptr<toggler> create(GLenum, bool);
};

template<typename argument>
class switcher : public toggler
{
  public:
    switcher(std::function<void(argument)> fun, std::vector<argument> vec)://exception if vec.size==0
        m_function(fun),
        m_args(vec),
        m_current(0)
    {}

    void callDefault() override
    {
        m_current = 0;
        m_function(m_args[m_current]);
    }

    void callNext() override
    {
        next();
        m_function(m_args[m_current]);
    }
    
    void next()
    {
        ++m_current;
        if(m_current >= m_args.size())
            m_current = 0;
    }
    

  private:
    std::function<void(argument)> m_function;
    std::vector<argument> m_args;
    unsigned int m_current;

};


class glEnabler : public toggler
{
  public:
    glEnabler(GLenum cap, bool en):
        m_capability(cap),
        m_default(en)
    {}
    
    void callDefault() override
    {
        if(m_default)
            glEnable(m_capability);
        else
            glDisable(m_capability);
    }

    void callNext() override
    {
        if(glIsEnabled(m_capability))
            glDisable(m_capability);
        else
            glEnable(m_capability);
    }

  private:
    GLenum m_capability;
    bool m_default;
};

class switchCollection
{
  public:
    bool toggle(const std::string& name)
    {
        auto reponse = m_content.find(name);
        if(reponse != m_content.end())
        {
            reponse->second->callNext();
            return true;
        }
        return false;
    }
    
  protected:
    std::unordered_map<std::string, std::shared_ptr<toggler> > m_content;
};
/*
class rasterSizeController
{
  public:
    rasterSizeController(GLfloat curr, GLfloat gran, GLfloat max):
        m_current(curr),
        m_granularity(gran),
        m_max(max)
    {}


    void check()
    {
        if(m_current <= 0)
            m_current = m_granularity;
        else if(m_current > m_max)
            m_current = m_max;
    }

    void update()
    {
        if(glGet(GL_POLYGON_MODE))
    }
    
  private:
    GLfloat m_granularity;
    };*/


class glSettings : public switchCollection
{
    using params=std::vector<GLuint>;
  public:
    glSettings()
    {
        m_content=
        {
            { "polygonMode", toggler::create( std::bind(glPolygonMode, GL_FRONT_AND_BACK, std::placeholders::_1),
                                              {GL_LINE, GL_FILL, GL_POINT})  },
            { "depthTest", toggler::create(GL_DEPTH_TEST, true)},
            { "faceCulled", toggler::create(glCullFace, {GL_BACK, GL_FRONT})},
            { "enableCullFace", toggler::create(GL_CULL_FACE, true)}//, { "rasterSize", toggler::creata( )}
        };
        for(auto it = m_content.begin(); it != m_content.end(); ++it)
        {
            it->second->callDefault();
        }
    }
    
    void changePrimitiveSize(bool incrementation) const;

};

}//namespace input
}//namespace gfg

#endif//MOOSS_GLTOGGLE_H
