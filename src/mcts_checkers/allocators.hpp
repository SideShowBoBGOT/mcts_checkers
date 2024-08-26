#pragma once

#include <span>

namespace mcts_checkers {

    class MonotonicMemoryResource {
        public:
            MonotonicMemoryResource()=default;
            explicit MonotonicMemoryResource(const std::span<std::byte>& storage)
                : m_storage(storage) {}
            std::byte* allocate(const std::size_t n) {
                const auto result = &m_storage[m_used];
                m_used += n;
                return result;
            }
            constexpr void release() noexcept { m_used = 0; }
            [[nodiscard]] constexpr std::size_t used() const noexcept { return m_used; }
        private:
            std::span<std::byte> m_storage{};
            std::size_t m_used = 0;
    };

    template <typename T>
    class MonotonicAllocator {
        public:
            using value_type = T;
            using size_type = std::size_t;
            using difference_type = std::ptrdiff_t;
            using propagate_on_container_move_assignment = std::true_type;

            template<typename U>
            friend class MonotonicAllocator;

            template <typename U>
                struct rebind {
                using other = MonotonicAllocator<U>;
            };

            MonotonicAllocator() noexcept = default;
            explicit MonotonicAllocator(MonotonicMemoryResource& memory_resource) noexcept
                : m_memory_resource(&memory_resource) {}

            template <typename U>
            explicit MonotonicAllocator(const MonotonicAllocator<U>& l) noexcept
                : m_memory_resource(l.m_memory_resource) {}

            T* allocate(const std::size_t n) {
                assert(m_memory_resource != nullptr);
                T* result = reinterpret_cast<T*>(m_memory_resource->allocate(n * sizeof(T)));
                return result;
            }
            static constexpr void deallocate(T*, const std::size_t) noexcept {}
        private:
            MonotonicMemoryResource* m_memory_resource = nullptr;
    };


    // template <typename T>
    // class VectorLinearAllocator {
    //     public:
    //         using value_type = T;
    //         using size_type = std::size_t;
    //         using difference_type = std::ptrdiff_t;
    //         using propagate_on_container_move_assignment = std::true_type;
    //
    //         template <typename U>
    //             struct rebind {
    //             using other = VectorLinearAllocator<U>;
    //         };
    //
    //         explicit VectorLinearAllocator(const size_t size) noexcept : m_storage(size) {}
    //         template <typename U> constexpr VectorLinearAllocator(const VectorLinearAllocator<U>&) noexcept {}
    //
    //         T* allocate(const std::size_t n) {
    //             T* result = reinterpret_cast<T*>(&m_storage[m_used]);
    //             m_used += n;
    //             return result;
    //         }
    //
    //         static constexpr void deallocate(T*, const std::size_t) noexcept {}
    //         [[nodiscard]] size_type max_size() const noexcept { return m_storage.size(); }
    //
    //         constexpr void deallocate_all() noexcept { m_used = 0; }
    //     private:
    //         std::vector<std::byte> m_storage;
    //         std::size_t m_used = 0;
    // };



}