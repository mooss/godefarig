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


#ifndef MOOSS_GL_BUFFER_H
#define MOOSS_GL_BUFFER_H

#include "gfg_utils.h"//glm::tvec3<T>
#include "glm/gtc/type_ptr.hpp"//glm::value_ptr
#include "myglad.h"
#include <vector>
#include <array>
#include <memory>//unique_ptr
#include <make_unique_cpp11.h>
#include <iostream>//<< redef

namespace gfg{
namespace gl{

template< class inside, std::size_t S>
const inside* fetch_data_ptr(const std::array<inside, S>& container)
{
    return container.data();
}

template< class inside, std::size_t S>
std::size_t dimension(const std::array<inside, S>& container)
{
    return S;
}

template< class inside>
const inside* fetch_data_ptr(const glm::tvec3<inside>& container)
{
    return glm::value_ptr(container);
}

template<class inside>
std::size_t fetch_data_size( std::vector<inside> const& container )
{
    return sizeof(container.front()) * container.size();
}

template<class inside>
std::size_t dimension(glm::tvec3<inside> const& container)
{
    return 3;
}

std::size_t gl_enum_type_size(GLenum value);

//this whole fetch_data_ptr and fetch_data_size shenanigan might be weirdly done but it's the best I can do for now

template<typename arrayType>//arrayType is supposed to be a std::vector or std::array or equivalent
class buffer_data 
{
  public:
    buffer_data(arrayType& holder, GLenum draw_mode=GL_STATIC_DRAW):
        data_holder_(holder),
        draw_mode_(draw_mode)
    {}

    std::size_t size() const
    {
        return fetch_data_size(  data_holder_ );
    }

    arrayType& holder() { return data_holder_; }
    arrayType const& holder() const { return data_holder_; }
    GLenum draw_mode() const { return draw_mode_; }

  private:
    arrayType& data_holder_;
    GLenum draw_mode_;
};//class buffer_data

class buffer_hint
{
  public:
    buffer_hint(GLuint index, GLenum type=GL_FLOAT, GLboolean normalised=GL_FALSE, GLvoid* pointer=(GLvoid*) 0);

    std::size_t type_size() const { return gl_enum_type_size(type_); }

    GLuint index() const { return index_; }
    GLenum type() const { return type_; }
    GLboolean normalised() const { return normalised_; }
    GLvoid* pointer() const { return pointer_; }
    
  private:
    GLuint index_;
    GLenum type_;
    GLboolean normalised_;
    GLvoid* pointer_;
};//class buffer_hint

class buffer
{
  public:
    buffer(GLenum target);
    virtual ~buffer();
    
    void bind() const;
    
    template<typename arrayType>
    void send(buffer_data<arrayType> const& data) const
    {

        bind();
        glBufferData(
            target_,
            data.size(),
            (void*) fetch_data_ptr( data.holder().front() ),
            data.draw_mode() );
    }

    //todo: update hint with data
    
    GLenum target() const { return target_; }
    GLuint handle() const { return handle_; }

  private:
    GLenum target_;

  protected:
    GLuint handle_;
};//class buffer

class vertex_buffer : public buffer
{
  public:
    vertex_buffer(buffer_hint const& hint);

    std::size_t type_size() const { return hint_.type_size(); }

    template<typename arrayType>
    void send(arrayType& container) const
    {
        buffer_data<arrayType> data(container);
        buffer::send(data);

        glVertexAttribPointer(
            hint_.index(),
            dimension( data.holder().front() ),
            hint_.type(),
            hint_.normalised(),
            hint_.type_size() * dimension( data.holder().front() ),
            hint_.pointer()
            );

        glEnableVertexAttribArray(hint_.index());
    }
  private:
    buffer_hint hint_;
};//class vertex_buffer

class element_buffer : public buffer//todo: move hint to vertex_buffer
{
  public:
    element_buffer();

    template< typename arrayType>
    void send( arrayType& container) const
    {
        bind();
        buffer_data<arrayType> data(container);
        
        glBufferData(
            target(),
            data.size(),
            (void*) fetch_data_ptr( data.holder().front() ),
            data.draw_mode() );
    }
};//class element_buffer

}//namespace gl
}//namespace gfg


std::ostream& operator<<(std::ostream& os, const gfg::gl::buffer_hint& hint);


template<typename arrayType>
std::ostream& operator<<(std::ostream& os, const gfg::gl::buffer_data<arrayType>& buf_data)
{
    os << "draw_mode=" << buf_data.draw_mode();
    return os;
}

std::ostream& operator<<(std::ostream& os, const gfg::gl::buffer& buf);

#endif//MOOSS_GL_BUFFER_H
