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
/*!\file    rrlib/buffer_pools/tGarbageFromDeletedBufferPools.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-09-30
 *
 * \brief   Contains tGarbageFromDeletedBufferPools
 *
 * \b tGarbageFromDeletedBufferPools
 *
 * Class for managing, collecting and deleting garbage resulting from deleted
 * buffer pools whose buffers are still in use.
 *
 */
//----------------------------------------------------------------------
#ifndef __rrlib__buffer_pools__tGarbageFromDeletedBufferPools_h__
#define __rrlib__buffer_pools__tGarbageFromDeletedBufferPools_h__

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
namespace deleting
{
template <typename TBufferManagementPolicy>
class CollectGarbage;
}

//----------------------------------------------------------------------
// Class declaration
//----------------------------------------------------------------------
//! Buffer pool deletion garbage management class
/*!
 * Class for managing, collecting and deleting garbage resulting from deleted
 * buffer pools whose buffers are still in use.
 *
 * When using CollectGarbage policy and deleting pools at application runtime,
 * garbage is collected in instances of this class.
 * Calling DeleteGarbage() will check if any pools
 * can be deleted safely now - and possibly do so.
 * Make sure to call this once in a while to prevent memory leaks.
 */
class tGarbageFromDeletedBufferPools
{

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  tGarbageFromDeletedBufferPools() {}

  virtual ~tGarbageFromDeletedBufferPools() {}

  /*!
   * To be overridden by subclass
   */
  virtual int DeleteBufferPoolGarbage() = 0;

  /*!
   * Check if any pools or buffers can be deleted safely now - and possibly do so.
   */
  static void DeleteGarbage();

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  template <typename TBufferManagementPolicy>
  friend class deleting::CollectGarbage;

  /*!
   * \param pool Buffer Pool that could not be deleted completely yet
   */
  static void AddPool(tGarbageFromDeletedBufferPools* pool);

};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
