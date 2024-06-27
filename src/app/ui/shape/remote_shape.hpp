#pragma once

#include <cstdint>

#include "red_black_tree.hpp"

namespace rmcs_referee::app::ui {
template <typename T>
class RemoteShape {
public:
    class SwappableDescriptor : private RedBlackTree<SwappableDescriptor>::Node {
    public:
        friend class RedBlackTree<SwappableDescriptor>;

        SwappableDescriptor()                                      = default;
        SwappableDescriptor(const SwappableDescriptor&)            = delete;
        SwappableDescriptor& operator=(const SwappableDescriptor&) = delete;
        SwappableDescriptor(SwappableDescriptor&&)                 = delete;
        SwappableDescriptor& operator=(SwappableDescriptor&& obj) noexcept {
            id_                       = obj.id_;
            obj.id_                   = 0;
            existence_confidence_     = obj.existence_confidence_;
            obj.existence_confidence_ = 0;
            return *this;
        };

        [[nodiscard]] bool has_id() const { return id_; }
        [[nodiscard]] bool try_assign_id() requires std::is_base_of_v<SwappableDescriptor, T>
                                                 && requires(T t) { t.after_swapped(); } {
            if (has_id()) [[unlikely]]
                return false;

            if (SwappableDescriptor* first = swapping_queue_.first()) {
                // Optimization: Try to find a swappable descriptor to avoid creating a new one
                swapping_queue_.erase(*first);
                *this = std::move(*first);
                // Notify derived class that is has been swapped
                static_cast<T*>(first)->after_swapped();
                return true;
            }

            if (max_id_ == 201) [[unlikely]]
                return false;
            else {
                id_ = ++max_id_;
                return true;
            }
        }
        [[nodiscard]] bool predict_try_assign_id(uint8_t& existence_confidence) const {
            if (has_id()) [[unlikely]]
                return false;

            if (SwappableDescriptor* first = swapping_queue_.first()) {
                existence_confidence = first->existence_confidence_;
                return true;
            }

            return max_id_ != 201;
        }

        [[nodiscard]] bool swapping_enabled() const {
            return !RedBlackTree<SwappableDescriptor>::Node::is_dangling();
        }
        void enable_swapping() {
            if (swapping_enabled())
                return;
            swapping_queue_.insert(*this);
        }
        void disable_swapping() {
            if (!swapping_enabled())
                return;
            swapping_queue_.erase(*this);
        }

        [[nodiscard]] uint8_t id() const { return id_; }
        [[nodiscard]] uint8_t existence_confidence() const { return existence_confidence_; }

        uint8_t increase_existence_confidence() {
            ++existence_confidence_;
            if (swapping_enabled()) {
                disable_swapping(), enable_swapping();
            }
            return existence_confidence_;
        }

    private:
        bool operator<(const SwappableDescriptor& obj) const {
            return existence_confidence_ < obj.existence_confidence_;
        }

        uint8_t id_                   = 0;
        uint8_t existence_confidence_ = 0;
    };

private:
    static inline uint8_t max_id_ = 0;
    static inline RedBlackTree<SwappableDescriptor> swapping_queue_;
};
} // namespace rmcs_referee::app::ui