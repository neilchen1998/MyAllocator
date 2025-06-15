#include <vector>   // std::vector
#include <memory>   // std::align
#include <cstddef>  // std::byte

template <typename T>
class MyBufferAllocator
{
public:

    // This is required for the allocator
    // source: https://en.cppreference.com/w/cpp/memory/allocator.html
    using value_type = T;
    using size_type = std::size_t;
    #if __cplusplus < 201703L
    using pointer = T*; // deprecated in C++17
    const_pointer = T*; // deprecated in C++17
    #endif

    /// @brief The default allocator constructor that sets all pointers to nullptr
    MyBufferAllocator() = default;

    /// @brief Constructs an allocator with a main buffer and an optional upstream buffer
    /// @param bufferPtr The start pointer of the main buffer
    /// @param bufferSz The size of the main buffer
    /// @param upstream The start pointer of the upstream buffer (backup buffer)
    MyBufferAllocator(std::byte* bufferPtr, std::size_t bufferSz, MyBufferAllocator* upstream = nullptr)
    : bufferStart_(bufferPtr),
    bufferEnd_(bufferPtr + bufferSz),
    curPtr_(bufferPtr),
    upstream_(upstream)
    {
    }

    template <typename U>
    MyBufferAllocator(const MyBufferAllocator<U>& other)
    : bufferStart_(other.bufferStart_),
    bufferEnd_(other.bufferEnd_),
    curPtr_(other.curPtr_),
    upstream_(nullptr)
    {
    }

    /// @brief The allocate function
    /// @param n The size of the new elements
    /// @return The pointer of the allocator
    T* allocate(std::size_t n)
    {
        if (n == 0) return nullptr;

        std::size_t alignment = alignof(value_type);
        std::size_t sz = n * sizeof(value_type);

        std::size_t space = bufferEnd_ - curPtr_;

        void* ptr = static_cast<void*>(curPtr_);
        if (std::align(alignment, sz, ptr, space))
        {
            std::byte* aligned = static_cast<std::byte*>(ptr);
            curPtr_ = aligned + sizeof(T) * n;
            return reinterpret_cast<T*>(aligned);
        }
        else if (upstream_)
        {
            return upstream_->allocate(n);
        }
        else
        {
            #if DEBUG
            fmt::print("default allocated!\n");
            // Or just throw a bad allocation error
            throw std::bad_alloc();
            #else
            // Rollback to the default allocator
            std::allocator<T> defaultAllocator;
            return defaultAllocator.allocate(n);
            #endif
        }

        return nullptr;
    }

    void deallocate(T* ptr, std::size_t n) noexcept
    {
        if (ptr == nullptr) return;
        #if DEBUG
        if (reinterpret_cast<std::byte*>(ptr) >= bufferStart_ && reinterpret_cast<std::byte*>(ptr) < bufferEnd_)
        {
            fmt::print("main buffer deallocated!\n");
        }
        else if (upstream_)
        {
            fmt::print("upstream deallocated!\n");
        }
        else
        {
            fmt::print("default deallocated!\n");
        }
        #else
        // Check if the current pointer falls between the buffer
        // if not then we first check if we have an upstream
        // if we do then we deallocate it
        // if not then we deallocate the default allocator
        if (reinterpret_cast<std::byte*>(ptr) >= bufferStart_ && reinterpret_cast<std::byte*>(ptr) < bufferEnd_)
        {
            // this is a no-op
        }
        else if (upstream_)
        {
            upstream_->deallocate(ptr, n);
        }
        else
        {
            std::allocator<T> defaultAllocator;
            defaultAllocator.deallocate(ptr, n);
        }
        #endif
    }

    /// @brief Erase all elements from the container by setting the current ptr to the start of the buffer
    void clear() noexcept
    {
        curPtr_ = bufferStart_;

        // Clear upstream
        if (upstream_)
        {
            upstream_->clear();
        }
    }

    /// @brief Returns true if both allocators have the same start and end pointers
    /// @tparam U The type of other allocator
    /// @param other Oter allocator
    /// @return True if both have same pointers
    template <typename U>
    bool operator==(const MyBufferAllocator<U>& other) const
    {
        return ((bufferStart_ == other.bufferStart_) && (bufferEnd_ == other.bufferEnd_));
    }

    /// @brief Returns true if both allocators are not the same (deprecated in C++17)
    /// @tparam U The type of other allocator
    /// @param other Oter allocator
    /// @return True if they do not have same pointers
    template<typename U>
    bool operator!=(const MyBufferAllocator<U>& other) const
    {
        return !(*this == other);
    }

    #if __cplusplus < 201703L
    /// @brief Allows you to create a new allocator instance that manages memory for a different type (deprecated in C++17)
    /// @tparam U A different type
    template <typename U>
    struct rebind {
        using other = MyBufferAllocator<U>;
    };
    #endif

private:
    std::byte* bufferStart_;
    std::byte* bufferEnd_;
    std::byte* curPtr_;
    MyBufferAllocator<T>* upstream_;
};
