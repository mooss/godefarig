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

#include "graphics/buffer.h"

gfg::gl::buffer::buffer(GLenum target):
    buffer(target, 1)
{}

gfg::gl::buffer::buffer(GLenum target, GLsizei size):
    target_(target)
    size_(size),
    handles_(size)
{
    glGenBuffers(size_, handles_.data());
}

gfg::gl::buffer::~buffer()
{
    glDeleteBuffers(size_, handles_.data());
}

gfg::gl::buffer::bind()
{
    bind(0);
}

gfg::gl::buffer::bind(unsigned int indice)
{
    glBindBuffer(target_, handles_[indice]);
}
