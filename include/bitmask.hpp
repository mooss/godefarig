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

#ifndef MOOSS_BITMASK_H
#define MOOSS_BITMASK_H

#include <type_traits>

//macro associating a mask, derived from bitmask, to its underlying option
#define CREATE_BITMASK_CONTEXT(mask, option) \
template<>                                   \
struct enable_bitmask_operators<option>      \
{                                            \
    static const bool enable = true;         \
};                                           \
template<>                                   \
struct bitmask_option<mask>                  \
{                                            \
    using underlying_enum = option;          \
};

namespace gfg{

/*
https://marcoarena.wordpress.com/2012/04/29/use-crtp-for-polymorphic-chaining/
http://blog.bitwigglers.org/using-enum-classes-as-type-safe-bitmasks/
*/

template<typename Enum>
struct enable_bitmask_operators
{
    static const bool enable = false;
};

template<typename T>
struct bitmask_option
{};

/**@brief the aim of this class is to provide a typesafe bitmask which
 * does not decay to an integral type
 *@tparam Derived the CRTP argument
 */
template<class Derived>
class bitmask
{
  public:
    using underlying_enum = typename bitmask_option<Derived>::underlying_enum;
    using underlying_type = typename std::underlying_type<underlying_enum>::type;

    /**@brief conversion constructor
     * @param opt the underlying_enum to convert
     */
    bitmask(underlying_enum opt):
        mask_(static_cast<underlying_type>(opt))
    {}

    // bitmask(underlying_type mask):
    //     mask_(mask)
    // {}

    bitmask() {}
    
    Derived& operator|=(const Derived& that)
    {
        mask_ |= that.mask_;
        return * static_cast<Derived*>(this);
    }

    Derived& operator&=(const Derived& that)
    {
        mask_ &= that.mask_;
        return * static_cast<Derived*>(this);
    }

    Derived& operator^=(const Derived& that)
    {
        mask_ ^= that.mask_;
        return * static_cast<Derived*>(this);
    }

    Derived operator|(Derived that) const 
    {
        that |= * static_cast<const Derived*>(this);
        return that;
    }

    Derived operator^(Derived that) const 
    {
        that ^= * static_cast<const Derived*>(this);
        return that;
    }

    Derived operator&(Derived that) const 
    {
        that &= * static_cast<const Derived*>(this);
        return that;
    }

    /**@brief toggle an option
     * @param opt the \ref underlying_enum to toggle
     */
    void toggle(underlying_enum opt)  { *this ^= opt; }
    /**@brief switches on an option
     * @param opt the \ref underlying_enum to switch on
     */
    void switch_on(underlying_enum opt) { *this |= opt; }
    /**@brief switches off an option
     * @param opt the \ref underlying_enum to switch off
     */
    void switch_off(underlying_enum opt) { *this &= ~opt; }

    /**@brief test if a mask is satisfied
     * @param that the \ref pression_mask against which to compare
    */
    bool satisfies(Derived that) const
    {
        return ( static_cast<const Derived*>(this)->mask_ & that.mask_) == that.mask_;
    }

    underlying_type value()
    {
        return mask_;
    }
  private:
    underlying_type mask_;
};//template<class Derived> class bitmask

template<typename Enum>  
typename std::enable_if<enable_bitmask_operators<Enum>::enable, Enum>::type  
operator |(Enum lhs, Enum rhs)  
{
    using underlying = typename std::underlying_type<Enum>::type;
    return static_cast<Enum> (
        static_cast<underlying>(lhs) |
        static_cast<underlying>(rhs)
    );
}

template<typename Enum>  
typename std::enable_if<enable_bitmask_operators<Enum>::enable, Enum>::type  
operator &(Enum lhs, Enum rhs)  
{
    using underlying = typename std::underlying_type<Enum>::type;
    return static_cast<Enum> (
        static_cast<underlying>(lhs) &
        static_cast<underlying>(rhs)
    );
}

template<typename Enum>  
typename std::enable_if<enable_bitmask_operators<Enum>::enable, Enum>::type  
operator ^(Enum lhs, Enum rhs)  
{
    using underlying = typename std::underlying_type<Enum>::type;
    return static_cast<Enum> (
        static_cast<underlying>(lhs) ^
        static_cast<underlying>(rhs)
    );
}

template<typename Enum>  
typename std::enable_if<enable_bitmask_operators<Enum>::enable, Enum>::type
operator~(Enum operande)
{
    using underlying = typename std::underlying_type<Enum>::type;
    return static_cast<Enum>( ~ static_cast<underlying>(operande));
}

// template<class Derived>
// Derived operator^(typename bitmask_option<Derived>::underlying_enum lhs, typename bitmask_option<Derived>::underlying_enum rhs)
// {
//     bitmask<Derived> result(lhs);
//     result ^= rhs;
//     return result;
// }

// template<class Derived>
// Derived operator&(typename bitmask_option<Derived>::underlying_enum lhs, typename bitmask_option<Derived>::underlying_enum rhs)
// {
//     bitmask<Derived> result(lhs);
//     result &= rhs;
//     return result;
// }

}//namespace gfg

#endif//MOOSS_BITMASK_H
