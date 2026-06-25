@page memory Architecture/Core/Systems

# Memory

The implementation for memory only uses the standart library malloc functions for actual memory allocations. The real purpose is to have a common allocator which is used by everything. This enables us track memory usage trivially, instead of having to dig around in the malloc stub. This functionality is disabled for the packaging version due to performance reasons.

## Regular Allocation Functions

There are the regular functions:

``` C++
// Engine/Core/System/Memory.h

static void* Allocate(uint64 size);
static void* AllocateAligned(uint64 size, uint64 alignment);
static void* Reallocate(void* ptr, uint64 size);
static void* ReallocateAligned(void* ptr, uint64 size, uint64 alignment);
static void Free(void* ptr) noexcept;
```

These functions are trivial, there is no need to explain them here.

## Construction function

For some purposes, you may want to use the constructor/destructor for a given class, which requires the new/delete operators which are explicitly forbidden. To restore the functionality, we use the following functions:

``` C++
// Engine/Core/System/Memory.h

template<class T, typename... Args>
static T* Construct(Args&&... args)

template<class T>
static void Destruct(T* p)
```

These functions call new/delete on your behalf.

