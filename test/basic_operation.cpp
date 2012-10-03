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
/*!\file    rrlib/buffer_pools/test/basic_operation.cpp
 *
 * \author  Max Reichardt
 *
 * \date    2012-10-01
 *
 * Test basic operation of buffer pools with various combinations of policies.
 */
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "rrlib/buffer_pools/tBufferPool.h"

//----------------------------------------------------------------------
// Debugging
//----------------------------------------------------------------------
#include <cassert>

//----------------------------------------------------------------------
// Namespace usage
//----------------------------------------------------------------------
using namespace rrlib::buffer_pools;
using namespace rrlib::concurrent_containers;

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Const values
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Implementation
//----------------------------------------------------------------------
class tTestType : public tQueueable, public tBufferManagementInfo
{
public:
  tTestType(const std::string& content) : content(content) {}
  std::string content;
};

inline std::ostream& operator << (std::ostream& output, const tTestType& t)
{
  output << t.content;
  return output;
}

template <typename T, typename TManaged, bool INSTANT_DELETE, typename TPool>
void TestBufferPool(TPool* pool)
{
  RRLIB_LOG_PRINT(USER, " Attaching 4 buffers");
  for (char i = '0'; i < '4'; ++i)
  {
    char name[2] = { i, 0 };
    pool->AddBuffer(std::unique_ptr<TManaged>(new TManaged(name)));
  }

  RRLIB_LOG_PRINT(USER, " Obtaining 5 buffers consecutively");
  for (int i = 0; i < 5; ++i)
  {
    RRLIB_LOG_PRINT(USER, "  Obtained buffer '", *pool->GetUnusedBuffer(), "'.");
  }

  RRLIB_LOG_PRINT(USER, " Obtaining 5 buffers simultaneously");
  std::vector<typename TPool::tPointer> buffer_pointers;
  for (int i = 0; i < 5; ++i)
  {
    typename TPool::tPointer ptr(pool->GetUnusedBuffer());
    if (ptr)
    {
      RRLIB_LOG_PRINT(USER, "  Obtained buffer '", *ptr, "'.");
      buffer_pointers.push_back(std::move(ptr));
    }
    else
    {
      RRLIB_LOG_PRINT(USER, "  Obtained no buffer. Allocating and adding another one.");
      ptr = pool->AddBuffer(std::unique_ptr<TManaged>(new TManaged("another buffer")));
      buffer_pointers.push_back(std::move(ptr));
    }
  }


  if (INSTANT_DELETE)
  {
    RRLIB_LOG_PRINT(USER, " Releasing buffers pointers");
    buffer_pointers.clear();
  }
  RRLIB_LOG_PRINT(USER, " Deleting pool");
  delete pool;
  if (!INSTANT_DELETE)
  {
    RRLIB_LOG_PRINT(USER, " Releasing buffers pointers");
    buffer_pointers.clear();
    tGarbageFromDeletedBufferPools::DeleteGarbage();
  }
}

template < typename T,
         bool INSTANT_DELETE,
         template <typename, tQueueConcurrency, typename ...> class TBufferManagementPolicy,
         template <typename> class TDeletingPolicy,
         template <typename, typename> class TRecycling,
         typename... TBufferManagementPolicyArgs >
void TestBufferPoolWithAllConcurrencyLevels(const char* pool_type_printf_string)
{
  RRLIB_LOG_PRINTF(USER, " ");
  {
    RRLIB_LOG_PRINTF(USER, pool_type_printf_string, "tQueueConcurrency::NONE");
    typedef tBufferPool<T, tQueueConcurrency::NONE, TBufferManagementPolicy, TDeletingPolicy, TRecycling, TBufferManagementPolicyArgs...> tPool;
    TestBufferPool<T, typename tPool::tManagedType, INSTANT_DELETE>(new tPool());
  }
  {
    RRLIB_LOG_PRINTF(USER, pool_type_printf_string, "tQueueConcurrency::SINGLE_READER_AND_WRITER_FAST");
    typedef tBufferPool<T, tQueueConcurrency::SINGLE_READER_AND_WRITER_FAST, TBufferManagementPolicy, TDeletingPolicy, TRecycling, TBufferManagementPolicyArgs...> tPool;
    TestBufferPool<T, typename tPool::tManagedType, INSTANT_DELETE>(new tPool());
  }
  {
    RRLIB_LOG_PRINTF(USER, pool_type_printf_string, "tQueueConcurrency::MULTIPLE_WRITERS");
    typedef tBufferPool<T, tQueueConcurrency::MULTIPLE_WRITERS, TBufferManagementPolicy, TDeletingPolicy, TRecycling, TBufferManagementPolicyArgs...> tPool;
    TestBufferPool<T, typename tPool::tManagedType, INSTANT_DELETE>(new tPool());
  }
  {
    RRLIB_LOG_PRINTF(USER, pool_type_printf_string, "tQueueConcurrency::MULTIPLE_WRITERS_FAST");
    typedef tBufferPool<T, tQueueConcurrency::MULTIPLE_WRITERS_FAST, TBufferManagementPolicy, TDeletingPolicy, TRecycling, TBufferManagementPolicyArgs...> tPool;
    TestBufferPool<T, typename tPool::tManagedType, INSTANT_DELETE>(new tPool());
  }
  {
    RRLIB_LOG_PRINTF(USER, pool_type_printf_string, "tQueueConcurrency::MULTIPLE_READERS_FAST");
    typedef tBufferPool<T, tQueueConcurrency::MULTIPLE_READERS_FAST, TBufferManagementPolicy, TDeletingPolicy, TRecycling, TBufferManagementPolicyArgs...> tPool;
    TestBufferPool<T, typename tPool::tManagedType, INSTANT_DELETE>(new tPool());
  }
  {
    RRLIB_LOG_PRINTF(USER, pool_type_printf_string, "tQueueConcurrency::FULL_FAST");
    typedef tBufferPool<T, tQueueConcurrency::FULL_FAST, TBufferManagementPolicy, TDeletingPolicy, TRecycling, TBufferManagementPolicyArgs...> tPool;
    TestBufferPool<T, typename tPool::tManagedType, INSTANT_DELETE>(new tPool());
  }
}

int main(int, char**)
{
  // Queue-based
  TestBufferPoolWithAllConcurrencyLevels<tTestType, true, management::QueueBased, deleting::ComplainOnMissingBuffers, recycling::StoreOwnerInUniquePointer>(
    "Testing tBufferPool<tTestType, %s, management::QueueBased, deleting::ComplainOnMissingBuffers, recycling::StoreOwnerInUniquePointer>:");
  TestBufferPoolWithAllConcurrencyLevels<tTestType, true, management::QueueBased, deleting::ComplainOnMissingBuffers, recycling::UseBufferContainer>(
    "Testing tBufferPool<tTestType, %s, management::QueueBased, deleting::ComplainOnMissingBuffers, recycling::UseBufferContainer>:");
  TestBufferPoolWithAllConcurrencyLevels<tTestType, true, management::QueueBased, deleting::ComplainOnMissingBuffers, recycling::UseOwnerStorageInBuffer>(
    "Testing tBufferPool<tTestType, %s, management::QueueBased, deleting::ComplainOnMissingBuffers, recycling::UseOwnerStorageInBuffer>:");

  TestBufferPoolWithAllConcurrencyLevels<tTestType, false, management::QueueBased, deleting::CollectGarbage, recycling::StoreOwnerInUniquePointer>(
    "Testing tBufferPool<tTestType, %s, management::QueueBased, deleting::CollectGarbage, recycling::StoreOwnerInUniquePointer>:");
  TestBufferPoolWithAllConcurrencyLevels<tTestType, false, management::QueueBased, deleting::CollectGarbage, recycling::UseBufferContainer>(
    "Testing tBufferPool<tTestType, %s, management::QueueBased, deleting::CollectGarbage, recycling::UseBufferContainer>:");
  TestBufferPoolWithAllConcurrencyLevels<tTestType, false, management::QueueBased, deleting::CollectGarbage, recycling::UseOwnerStorageInBuffer>(
    "Testing tBufferPool<tTestType, %s, management::QueueBased, deleting::CollectGarbage, recycling::UseOwnerStorageInBuffer>:");

  // Array-based
  TestBufferPoolWithAllConcurrencyLevels<std::string, true, management::ArrayAndFlagBased, deleting::ComplainOnMissingBuffers, recycling::StoreOwnerInUniquePointer>(
    "Testing tBufferPool<std::string, %s, management::ArrayAndFlagBased, deleting::ComplainOnMissingBuffers, recycling::StoreOwnerInUniquePointer>:");
  TestBufferPoolWithAllConcurrencyLevels<std::string, true, management::ArrayAndFlagBased, deleting::ComplainOnMissingBuffers, recycling::UseBufferContainer>(
    "Testing tBufferPool<std::string, %s, management::ArrayAndFlagBased, deleting::ComplainOnMissingBuffers, recycling::UseBufferContainer>:");
  TestBufferPoolWithAllConcurrencyLevels<tTestType, true, management::ArrayAndFlagBased, deleting::ComplainOnMissingBuffers, recycling::UseOwnerStorageInBuffer>(
    "Testing tBufferPool<tTestType, %s, management::ArrayAndFlagBased, deleting::ComplainOnMissingBuffers, recycling::UseOwnerStorageInBuffer>:");

  TestBufferPoolWithAllConcurrencyLevels<std::string, false, management::ArrayAndFlagBased, deleting::CollectGarbage, recycling::StoreOwnerInUniquePointer>(
    "Testing tBufferPool<std::string, %s, management::ArrayAndFlagBased, deleting::CollectGarbage, recycling::StoreOwnerInUniquePointer>:");
  TestBufferPoolWithAllConcurrencyLevels<std::string, false, management::ArrayAndFlagBased, deleting::CollectGarbage, recycling::UseBufferContainer>(
    "Testing tBufferPool<std::string, %s, management::ArrayAndFlagBased, deleting::CollectGarbage, recycling::UseBufferContainer>:");
  TestBufferPoolWithAllConcurrencyLevels<tTestType, false, management::ArrayAndFlagBased, deleting::CollectGarbage, recycling::UseOwnerStorageInBuffer>(
    "Testing tBufferPool<tTestType, %s, management::ArrayAndFlagBased, deleting::CollectGarbage, recycling::UseOwnerStorageInBuffer>:");
}
