//
// You received this file as part of RRLib
// Robotics Research Library
//
// Copyright (C) Finroc GbR (finroc.org)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
//----------------------------------------------------------------------
/*!\file    rrlib/buffer_pools/tNotifyOnRecycle.h
 *
 * \author  Max Reichardt
 *
 * \date    2013-04-30
 *
 * \brief   Contains tNotifyOnRecycle
 *
 * \b tNotifyOnRecycle
 *
 * Types that are subclasses of this, are notified whenever they are recycled
 * and returned to a buffer pool.
 *
 * They need some method [virtual] void OnRecycle()
 *
 */
//----------------------------------------------------------------------
#ifndef __rrlib__buffer_pools__tNotifyOnRecycle_h__
#define __rrlib__buffer_pools__tNotifyOnRecycle_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Namespace declaration
//----------------------------------------------------------------------
namespace rrlib
{
namespace buffer_pools
{

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Class declaration
//----------------------------------------------------------------------
//! Super class for types to be notified on recycling
/*!
 * Types that are subclasses of this, are notified whenever they are recycled
 * and returned to a buffer pool.
 *
 * They need some method "[virtual] void OnRecycle()".
 * This is called whenever this object is recycled.
 */
class tNotifyOnRecycle
{
};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
