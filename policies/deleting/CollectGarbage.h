//
// You received this file as part of RRLib
// Robotics Research Library
//
// Copyright (C) Finroc GbR (finroc.org)
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
//----------------------------------------------------------------------
/*!\file    rrlib/buffer_pools/policies/deleting/CollectGarbage.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-09-30
 *
 * \brief   Contains CollectGarbage
 *
 * \b CollectGarbage
 *
 * Deleting policy that will keep pool in memory if buffers are still in use.
 * Pools that could not be deleted are collected in a set.
 * Calling DeleteGarbage() will check if any of these pools can be deleted safely now - and possibly do so.
 * When using this policy and deleting pools at application runtime,
 * make sure to call this once in a while to prevent memory leaks.
 */
//----------------------------------------------------------------------
#ifndef __rrlib__buffer_pools__policies__deleting__CollectGarbage_h__
#define __rrlib__buffer_pools__policies__deleting__CollectGarbage_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "rrlib/buffer_pools/tGarbageFromDeletedBufferPools.h"

//----------------------------------------------------------------------
// Namespace declaration
//----------------------------------------------------------------------
namespace rrlib
{
namespace buffer_pools
{
namespace deleting
{

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Class declaration
//----------------------------------------------------------------------
//! Collect pools whose buffers are still in use
/*!
 * Deleting policy that will keep pool in memory if buffers are still in use.
 * Pools that could not be deleted are collected in tGarbageFromDeletedBufferPools objects.
 *
 * Calling tGarbageFromDeletedBufferPools::DeleteGarbage() will check if any of these pools
 * can be deleted safely now - and possibly do so.
 * When using this policy and deleting pools at application runtime,
 * make sure to call this once in a while to prevent memory leaks.
 */
template <typename TBufferManagementPolicy>
class CollectGarbage : boost::noncopyable
{

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  CollectGarbage() : buffer_management(new TBufferManagementPolicy()) {}

  ~CollectGarbage()
  {
    int missing_buffers = buffer_management->DeleteGarbage();
    if (missing_buffers <= 0)
    {
      delete buffer_management;
    }
    else
    {
      tGarbageFromDeletedBufferPools::AddPool(new tGarbage(*buffer_management));
    }
  }

  TBufferManagementPolicy& GetBufferManagement()
  {
    return *buffer_management;
  }

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  /*!
   * Buffer management object.
   * Allocated seperately so that it can exist longer than buffer pool.
   */
  TBufferManagementPolicy* buffer_management;

  class tGarbage : public tGarbageFromDeletedBufferPools
  {
  public:
    tGarbage(TBufferManagementPolicy& buffer_management) : buffer_management(buffer_management) {}

    virtual ~tGarbage()
    {
      delete &buffer_management;
    }

  private:
    virtual int DeleteBufferPoolGarbage()
    {
      return buffer_management.DeleteGarbage();
    }

    TBufferManagementPolicy& buffer_management;
  };

};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
}


#endif
