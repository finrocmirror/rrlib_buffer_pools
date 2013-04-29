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
/*!\file    rrlib/buffer_pools/policies/management/ArrayAndFlagBased.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-09-30
 *
 * \brief   Contains ArrayAndFlagBased
 *
 * \b ArrayAndFlagBased
 *
 * Buffers are stored in an array list.
 * Whether buffers are in use is signaled by a flag.
 */
//----------------------------------------------------------------------
#ifndef __rrlib__buffer_pools__policies__management__ArrayAndFlagBased_h__
#define __rrlib__buffer_pools__policies__management__ArrayAndFlagBased_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include "rrlib/thread/tThread.h"
#include <array>

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
namespace management
{

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Class declaration
//----------------------------------------------------------------------
//! Array and flag-based buffer management
/*!
 * Buffers are stored in an array list.
 * Whether buffers are in use is signaled by a flag.
 *
 * Pro: Any type T can be used
 * Con: May not scale well with many buffers
 *
 * TAddMutex Mutex to protect AddBuffer operation with (may be tNoMutex if concurrent adding does not occur)
 */
template < typename T,
         concurrent_containers::tConcurrency CONCURRENCY,
         typename TBufferDeleter,
         typename TAddMutex = typename std::conditional < (CONCURRENCY == concurrent_containers::tConcurrency::FULL) || (CONCURRENCY == concurrent_containers::tConcurrency::MULTIPLE_READERS), thread::tMutex, thread::tNoMutex >::type >
class ArrayAndFlagBased : public TAddMutex
{
  enum { cMULTIPLE_READERS = (CONCURRENCY == concurrent_containers::tConcurrency::FULL) || (CONCURRENCY == concurrent_containers::tConcurrency::MULTIPLE_READERS) };
  enum { cATOMIC_ARRAY_ELEMENTS = (CONCURRENCY != concurrent_containers::tConcurrency::NONE) };
  enum { cARRAY_CHUNK_SIZE = 15 }; // TODO make this template argument
  typedef typename std::conditional<cATOMIC_ARRAY_ELEMENTS, std::atomic<T*>, T*>::type tArrayElement;
  struct tArrayChunk;
  typedef typename std::conditional<cMULTIPLE_READERS, std::atomic<tArrayChunk*>, tArrayChunk*>::type tNextArrayChunkPointer;
  typedef typename std::conditional<cMULTIPLE_READERS, std::atomic<int>, int>::type tBufferCount;

  /*! The 'array' is a linked list of array chunks */
  struct tArrayChunk
  {
    /*! Buffers in array chunk. NULL for buffers that are in use. */
    std::array<tArrayElement, cARRAY_CHUNK_SIZE> buffers;

    /*! Pointer to next chunk -> linked-list */
    tNextArrayChunkPointer next_chunk;

    ~tArrayChunk()
    {
      tArrayChunk* next = next_chunk;
      delete next;
    }
  };

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  ArrayAndFlagBased() :
    first_array_chunk(), buffer_count(0)
  {}

  void AddBuffer(T* buffer, tBufferManagementInfo& info)
  {
    thread::tLock lock(*this);
    int count = buffer_count;
    tArrayChunk* current = &first_array_chunk;
    while (count >= cARRAY_CHUNK_SIZE)
    {
      count -= cARRAY_CHUNK_SIZE;
      tArrayChunk* next = current->next_chunk;
      if (next)
      {
        current = next;
      }
      else
      {
        // slightly verbose as current->next_chunk might be atomic
        next = new tArrayChunk();
        current->next_chunk = next;
        current = next;
      }
    }
    //current->buffers[count] = buffer; // will be done by recycler
    info.buffer_management_info = &(current->buffers[count]);
    buffer_count = count + 1; // more efficient than ++-operator - safe due to lock
  }

  /*!
   * \return Number of buffers that have not been returned yet
   */
  int DeleteGarbage()
  {
    thread::tLock lock(*this); // should not be necessary, if pool is used sensibly, but does not hurt
    int remaining_buffers = this->buffer_count;
    tArrayChunk* current = &first_array_chunk;
    while (remaining_buffers > 0)
    {
      for (auto it = current->buffers.begin(); (it != current->buffers.end()) && (remaining_buffers > 0); ++it, remaining_buffers--)
      {
        T* buffer = (*it);
        if (buffer)
        {
          TBufferDeleter deleter;
          deleter(buffer);
          this->buffer_count--;
        }
      }
      current = current->next_chunk;
    }
    return this->buffer_count;
  }

  T* GetUnusedBuffer(tBufferManagementInfo& info)
  {
    int remaining_buffers = this->buffer_count;
    tArrayChunk* current = &first_array_chunk;
    while (remaining_buffers > 0)
    {
      for (auto it = current->buffers.begin(); (it != current->buffers.end()) && (remaining_buffers > 0); ++it, remaining_buffers--)
      {
        T* buffer = (*it);
        if (buffer)
        {
          tArrayElement* array_entry = &(*it);
          if (MarkBufferUsed(*array_entry, buffer)) // write NULL to array to indicate that buffer is used
          {
            info.buffer_management_info = array_entry;
            return buffer;
          }
        }
      }
      current = current->next_chunk;
    }
    info.buffer_management_info = NULL;
    return NULL;
  }

  static void RecycleBuffer(const tBufferManagementInfo& info, T* buffer)
  {
    assert(info.buffer_management_info && "Received empty buffer_management_info. This is not allowed using this policy.");
    tArrayElement* array_entry = static_cast<tArrayElement*>(info.buffer_management_info);
    *array_entry = buffer; // restore pointer (NULL -> buffer pointer)
  }

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  /*! First array chunk in 'array' */
  tArrayChunk first_array_chunk;

  /*! Number of buffers in this pool */
  tBufferCount buffer_count;

  template <bool MULTIPLE_READERS = cMULTIPLE_READERS>
  bool MarkBufferUsed(tArrayElement& array_element, typename std::enable_if < !MULTIPLE_READERS, T >::type* buffer)
  {
    array_element = NULL;
    return true;
  }

  template <bool MULTIPLE_READERS = cMULTIPLE_READERS>
  bool MarkBufferUsed(tArrayElement& array_element, typename std::enable_if<MULTIPLE_READERS, T>::type* buffer)
  {
    return array_element.compare_exchange_strong(buffer, NULL);
  }
};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
}


#endif
