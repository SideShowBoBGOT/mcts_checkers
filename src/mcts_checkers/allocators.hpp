#pragma once

#include <span>

namespace mcts_checkers {

    class MonotonicMemoryResource {
        public:
            constexpr MonotonicMemoryResource()=default;
            explicit constexpr MonotonicMemoryResource(const std::span<std::byte>& storage)
                : m_storage(storage) {}
            constexpr std::byte* allocate(const std::size_t n) {
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

            constexpr MonotonicAllocator() noexcept = default;
            constexpr explicit MonotonicAllocator(MonotonicMemoryResource& memory_resource) noexcept
                : m_memory_resource(&memory_resource) {}

            template <typename U>
            constexpr explicit MonotonicAllocator(const MonotonicAllocator<U>& l) noexcept
                : m_memory_resource(l.m_memory_resource) {}

            constexpr T* allocate(const std::size_t n) {
                assert(m_memory_resource != nullptr);
                T* result = reinterpret_cast<T*>(m_memory_resource->allocate(n * sizeof(T)));
                return result;
            }
            static constexpr void deallocate(T*, const std::size_t) noexcept {}
        private:
            MonotonicMemoryResource* m_memory_resource = nullptr;
    };
}