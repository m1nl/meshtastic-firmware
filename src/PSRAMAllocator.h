/// \copyright
/// Copyright 2021 Mike Dunston (https://github.com/atanisoft)
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// \file psram_allocator.h
/// This file declares an allocator that provides memory from PSRAM rather than
/// internal memory.

#pragma once

#ifdef ARCH_ESP32
#include "sdkconfig.h"
#endif

#include <Arduino.h>
#include <assert.h>

template <class T> class PSRAMAllocator
{
  public:
    using value_type = T;

    PSRAMAllocator() noexcept {}

    template <class U> constexpr PSRAMAllocator(const PSRAMAllocator<U> &) noexcept {}

    [[nodiscard]] value_type *allocate(std::size_t n)
    {
        T *p = nullptr;

#if defined(CONFIG_SPIRAM) && defined(ARCH_ESP32) && defined(BOARD_HAS_PSRAM)
        // attempt to allocate in PSRAM first
        p = static_cast<value_type *>(ps_calloc(n, sizeof(value_type)));

        // If the allocation in PSRAM failed (or PSRAM not enabled), try to
        // allocate from the default memory pool.
        if (p == nullptr) {
#endif // defined(CONFIG_SPIRAM) && defined(ARCH_ESP32) && defined(BOARD_HAS_PSRAM)
            p = static_cast<value_type *>(calloc(n, sizeof(value_type)));
#if defined(CONFIG_SPIRAM) && defined(ARCH_ESP32) && defined(BOARD_HAS_PSRAM)
        }
#endif // defined(CONFIG_SPIRAM) && defined(ARCH_ESP32) && defined(BOARD_HAS_PSRAM)
        assert(p);

        return p;
    }

    void deallocate(value_type *p, std::size_t) noexcept { free(p); }
};
template <class T, class U> bool operator==(const PSRAMAllocator<T> &, const PSRAMAllocator<U> &)
{
    return true;
}
template <class T, class U> bool operator!=(const PSRAMAllocator<T> &x, const PSRAMAllocator<U> &y)
{
    return !(x == y);
}