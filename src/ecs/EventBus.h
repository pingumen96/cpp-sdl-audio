#pragma once

#include <unordered_map>
#include <vector>
#include <memory>
#include <typeindex>
#include <mutex>
#include <functional>
#include <any>

namespace ecs {

    /**
     * @brief Thread-safe event bus for ECS communication
     *
     * Provides a centralized mechanism for systems to communicate through events.
     * Events are stored by type and can be emitted and consumed by systems.
     */
    class EventBus {
    private:
        // Storage for events by type
        std::unordered_map<std::type_index, std::vector<std::any>> mEvents;

        // Optional logging hook for serialization/replay
        std::function<void(const std::type_index&, const std::any&)> mLoggingHook;

        // Thread safety
        mutable std::mutex mMutex;

    public:
        EventBus() = default;
        ~EventBus() = default;

        // Non-copyable, non-movable for safety
        EventBus(const EventBus&) = delete;
        EventBus& operator=(const EventBus&) = delete;
        EventBus(EventBus&&) = delete;
        EventBus& operator=(EventBus&&) = delete;

        /**
         * @brief Emit an event of type T
         * @tparam T Event type
         * @tparam Args Constructor argument types
         * @param args Arguments to construct the event
         */
        template<typename T, typename... Args>
        void emit(Args&&... args) {
            std::lock_guard<std::mutex> lock(mMutex);

            auto typeIndex = std::type_index(typeid(T));
            T event(std::forward<Args>(args)...);

            // Call logging hook if set
            if (mLoggingHook) {
                mLoggingHook(typeIndex, std::make_any<T>(event));
            }

            mEvents[typeIndex].emplace_back(std::make_any<T>(std::move(event)));
        }

        /**
         * @brief Read all events of type T
         * @tparam T Event type
         * @return Vector of events of type T
         */
        template<typename T>
        std::vector<T> read() {
            std::lock_guard<std::mutex> lock(mMutex);

            auto typeIndex = std::type_index(typeid(T));
            std::vector<T> result;

            auto it = mEvents.find(typeIndex);
            if (it != mEvents.end()) {
                result.reserve(it->second.size());

                for (const auto& anyEvent : it->second) {
                    try {
                        result.push_back(std::any_cast<T>(anyEvent));
                    } catch (const std::bad_any_cast& e) {
                        // This should not happen if used correctly
                        // Log error if logging is available
                    }
                }
            }

            return result;
        }

        /**
         * @brief Read all events of type T and clear them
         * @tparam T Event type
         * @return Vector of events of type T
         */
        template<typename T>
        std::vector<T> readAndClear() {
            std::lock_guard<std::mutex> lock(mMutex);

            auto typeIndex = std::type_index(typeid(T));
            std::vector<T> result;

            auto it = mEvents.find(typeIndex);
            if (it != mEvents.end()) {
                result.reserve(it->second.size());

                for (const auto& anyEvent : it->second) {
                    try {
                        result.push_back(std::any_cast<T>(anyEvent));
                    } catch (const std::bad_any_cast& e) {
                        // This should not happen if used correctly
                    }
                }

                // Clear the events after reading
                it->second.clear();
            }

            return result;
        }

        /**
         * @brief Clear all events of type T
         * @tparam T Event type
         */
        template<typename T>
        void clear() {
            std::lock_guard<std::mutex> lock(mMutex);

            auto typeIndex = std::type_index(typeid(T));
            auto it = mEvents.find(typeIndex);
            if (it != mEvents.end()) {
                it->second.clear();
            }
        }

        /**
         * @brief Clear all events
         */
        void clearAll() {
            std::lock_guard<std::mutex> lock(mMutex);
            mEvents.clear();
        }

        /**
         * @brief Set a logging hook for event serialization/replay
         * @param hook Function to call when events are emitted
         */
        void setLoggingHook(std::function<void(const std::type_index&, const std::any&)> hook) {
            std::lock_guard<std::mutex> lock(mMutex);
            mLoggingHook = std::move(hook);
        }

        /**
         * @brief Get the number of events of type T
         * @tparam T Event type
         * @return Number of events
         */
        template<typename T>
        size_t getEventCount() const {
            std::lock_guard<std::mutex> lock(mMutex);

            auto typeIndex = std::type_index(typeid(T));
            auto it = mEvents.find(typeIndex);
            return (it != mEvents.end()) ? it->second.size() : 0;
        }

        /**
         * @brief Get total number of events across all types
         * @return Total event count
         */
        size_t getTotalEventCount() const {
            std::lock_guard<std::mutex> lock(mMutex);

            size_t total = 0;
            for (const auto& pair : mEvents) {
                total += pair.second.size();
            }
            return total;
        }
    };

} // namespace ecs
