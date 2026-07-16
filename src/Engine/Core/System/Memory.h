#pragma once

// This file does not contain LLM generated documentation

#include <Engine/Types/CommonTypes.h>
#include <unordered_map>

/**
 * This is a custom memory interface that is meant to track overall memory usage.
 * @note This uses the malloc allocator internally.
 */
class WAllocator
{
public:
    /**
     * Starts up the allocator. This must only ever be called at startup.
     */
    static void BootAllocator();

    /**
     * Adds untracked memory to the usage.
     * @param size Size of the allocation.
     * @note Only use this function if a library does not support a custom allocator.
     */
    static void ReportExternalAllocation(uint64 size) noexcept;
    /**
     * Removes untracked memory to the usage.
     * @param size Size of the free.
     * @note Only use this function if a library does not support a custom allocator.
     */
    static void ReportExternalFree(uint64 size) noexcept;

    /**
     * Allocated memory and tracks usage.
     * @param size Size of the allocation.
     * @return Pointer to the allocated memory.
     */
    static void* Allocate(uint64 size);

    /**
     * Allocated memory with allocation requirements and tracks usage.
     * @param size Size of the allocation.
     * @param alignment Alignment requirements.
     * @return Pointer to the allocated memory.
     */
    static void* AllocateAligned(uint64 size, uint64 alignment);

    /**
     * Reallocated memory and tracks usage.
     * @param ptr Pointer to allocated memory.
     * @param size New size for the memory.
     * @return Pointer to the reallocated memory.
     */
    static void* Reallocate(void* ptr, uint64 size);

    /**
     * Reallocated memory with allocation requirements and tracks usage.
     * @param ptr Pointer to allocated memory.
     * @param size New size for the memory.
     * @param alignment Alignment requirements.
     * @return Pointer to the reallocated memory.
     */
    static void* ReallocateAligned(void* ptr, uint64 size, uint64 alignment);

    /**
     * Deallocated memory and tracks usage.
     * @param ptr Pointer to be deallocated.
     */
    static void Free(void* ptr) noexcept;

    /**
     * Constructs an object and tracks usage.
     * @tparam T Type of the object.
     * @tparam Args Constructor argument type list.
     * @param args Constructor argument list.
     * @return Constructed object.
     */
    template<class T, typename... Args>
    static T* Construct(Args&&... args)
    {
        uint64 size = sizeof(T);
        MemoryUsed() += size;
        T* obj = new T(std::forward<Args>(args)...);

        MemorySizes()[obj] = size;
        return obj;
    }

    /**
     * Destructs the object and tracks usage.
     * @tparam T Type of the object.
     * @param p Pointer to the object.
     */
    template<class T>
    static void Destruct(T* p)
    {
        auto it = MemorySizes().find(p);
        MemorySizes().erase(it);
        MemoryUsed() -= it->second;
        delete p;
    }

    /**
     * Gets tracked memory usage.
     * @return Memory usage in bytes.
     */
    static uint64 GetMemoryUsage();

private:
    static std::unordered_map<void*, uint64>& MemorySizes()
    {
        static std::unordered_map<void*, uint64> map;
        return map;
    }

    static uint64& MemoryUsed()
    {
        static uint64 used = 0;
        return used;
    }
};

#define wNew(size) WAllocator::Allocate(size);
#define wNewArr(type, count) (type*)WAllocator::Allocate(count * sizeof(type));
#define wFree(ptr) WAllocator::Free(ptr);