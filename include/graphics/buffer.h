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
#include "pointer_horror.hpp"

//#define DEBUG_MODE
#include "debug.h"

namespace gfg{
namespace gl{

std::size_t gl_enum_type_size(GLenum value);
std::string to_str(GLenum value);

/** \brief sequential data destined to be send to a buffer
 * \tparam arrayType structure holding the data
 */
template<typename arrayType>//arrayType is supposed to be a std::vector or std::array or equivalent
class buffer_data 
{
  public:
    /** \param holder structure holding the data
     * \param draw_mode expected usage pattern, corresponding to the fourth argument of the glBufferData function
     */
    buffer_data(arrayType& holder, GLenum draw_mode=GL_STATIC_DRAW):
        data_holder_(holder),
        draw_mode_(draw_mode)
    {}

    /** \brief data size getter
     * \return the size of the data
     */
    std::size_t size() const
    {
        return fetch_data_size(  data_holder_ );
    }

    /** \brief data holder getter
     * \return the data holder
     */
    arrayType& holder() { return data_holder_; }

    /** \brief data holder getter - const version
     */
    arrayType const& holder() const { return data_holder_; }

    /** \brief draw mode getter
     * \return the draw mode
     */
    GLenum draw_mode() const { return draw_mode_; }

  private:
    arrayType& data_holder_;///< structure holding the data
    GLenum draw_mode_;///< usage pattern. fourth argument to glBufferData
};//class buffer_data

/** \brief outputs buffer_data information to a stream
 * \tparam arrayType deducted buffer_data template parameter
 * \param os output stream
 * \param buf_data buffer data
 * \return the updated stream
 */
template<typename arrayType>
std::ostream& operator<<(std::ostream& os, const gfg::gl::buffer_data<arrayType>& buf_data)
{
    os << "draw_mode=" << buf_data.draw_mode()
       << " dimension=" << dimension(buf_data.holder())
       << " size=" << fetch_data_size(buf_data.holder())
       << " ptr=" << fetch_data_ptr(buf_data.holder());
    return os;
}

/** \brief light abstraction constructed around an opengl buffer
 */
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

        DEBUG("sending buffer data\n\tglBufferData(\n\t\t"
              << to_str(target_) << ",\n\t\t"
              << data.size() << ",\n\t\t"
              << fetch_data_ptr( data.holder()) << ",\n\t\t"
              << to_str(data.draw_mode()) << ");");
        glBufferData(
            target_,
            data.size(),
            (void*) fetch_data_ptr( data.holder()),
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

/** \brief contains informations about how to send an array of data to a vertex attribute
 *
 * Said data is to be used for calls to the function glVertexAttribPointer
 */
class vertex_buffer_hint
{
  public:
    vertex_buffer_hint(GLuint index, GLenum type=GL_FLOAT, GLboolean normalised=GL_FALSE, GLvoid* pointer=(GLvoid*) 0);

    /** \brief size of the associated type
        \return the size of the type associated to this instance
     */
    std::size_t type_size() const { return gl_enum_type_size(type_); }

    /** \brief index_ getter
     */
    GLuint index() const { return index_; }
    
    /** \brief type_ getter
     */
    GLenum type() const { return type_; }

    /** \brief normalised_ getter
     */
    GLboolean normalised() const { return normalised_; }

    /** \brief pointer_ getter
     */
    GLvoid* pointer() const { return pointer_; }
    
  private:
    GLuint index_;///< index of the vertex attribute to be modified by glVertexAttribPointer
    GLenum type_;///< type of the data to be send
    GLboolean normalised_;///< specifies whether the data should be normalised
    GLvoid* pointer_;///< pointer to the first component of the first vertex attribute in the array
};//class vertex_buffer_hint

class vertex_buffer : public buffer
{
  public:
    vertex_buffer(vertex_buffer_hint const& hint);

    std::size_t type_size() const { return hint_.type_size(); }
    const vertex_buffer_hint& hint() const { return hint_; }

    template<typename arrayType>
    void send(arrayType& container) const
    {
        buffer_data<arrayType> data(container);
        buffer::send(data);

        DEBUG("\tglVertexAttribPointer(\n\t\t"
              << hint_.index() << ",\n\t\t"
              << dimension( data.holder()) << ",\n\t\t"
              << to_str(hint_.type()) << ",\n\t\t"
              << to_str(hint_.normalised()) << ",\n\t\t"
              << hint_.type_size() * dimension( data.holder()) << ",\n\t\t"
              << hint_.pointer() << ");");

        glVertexAttribPointer(
            hint_.index(),
            dimension( data.holder() ),
            hint_.type(),
            hint_.normalised(),
            hint_.type_size() * dimension( data.holder() ),
            hint_.pointer()
            );

        glEnableVertexAttribArray(hint_.index());
    }

    template<typename arrayType>
    void send(arrayType& container, unsigned int dimension) const
    {
        buffer_data<arrayType> data(container);
        buffer::send(data);

        DEBUG("\tglVertexAttribPointer(\n\t\t"
              << hint_.index() << ",\n\t\t"
              << dimension << ",\n\t\t"
              << to_str(hint_.type()) << ",\n\t\t"
              << to_str(hint_.normalised()) << ",\n\t\t"
              << hint_.type_size() * dimension << ",\n\t\t"
              << hint_.pointer() << ");");

        glVertexAttribPointer(
            hint_.index(),
            dimension,
            hint_.type(),
            hint_.normalised(),
            hint_.type_size() * dimension,
            hint_.pointer()
            );

        glEnableVertexAttribArray(hint_.index());
    }

  private:
    vertex_buffer_hint hint_;
};//class vertex_buffer

class element_buffer : public buffer
{
  public:
    element_buffer();

    template< typename arrayType>
    void send( arrayType& container) const
    {
        bind();
        buffer_data<arrayType> data(container);

        DEBUG("sending element data\n\tglBufferData(\n\t\t"
              << to_str(target()) << ",\n\t\t"
              << data.size() << ",\n\t\t"
              << fetch_data_ptr( data.holder() ) << ",\n\t\t"
              << to_str(data.draw_mode()) << ");");

        glBufferData(
            target(),
            data.size(),
            (void*) fetch_data_ptr( data.holder() ),
            data.draw_mode() );
    }

    // template<typename indice_type>
    // void send( const gsl::span<indice_type>& container )
    // {
        
    // }
    
};//class element_buffer

}//namespace gl
}//namespace gfg


std::ostream& operator<<(std::ostream& os, const gfg::gl::vertex_buffer_hint& hint);
std::ostream& operator<<(std::ostream& os, const gfg::gl::buffer& buf);
std::ostream& operator<<(std::ostream& os, const gfg::gl::vertex_buffer& buf);

#endif//MOOSS_GL_BUFFER_H
