#pragma once

// using function templates:
// swap(T& a, T& b) – swap two values
// clamp(T value, T min, T max) – clamp value between min and max
// max3(T a, T b, T c) – return the maximum of three values
// always use const& where possible for efficiency

#include <algorithm>
#include <concepts>
#include <utility>

namespace utils {

    /** @brief Swaps two values of type T.
     * @param a Riferimento al primo valore.
     * @param b Riferimento al secondo valore.
     */
    template <std::movable T>
    constexpr void swap(T& a, T& b) noexcept {
        T temp = std::move(a);
        a = std::move(b);
        b = std::move(temp);
    }

    /** @brief Clamps a value between a minimum and maximum.
     * @param value Valore da clampare.
     * @param min Limite inferiore.
     * @param max Limite superiore.
     * @return Valore clampato tra min e max.
     */
    template <std::totally_ordered T>
    [[nodiscard]] constexpr T clamp(const T& value, const T& min, const T& max) noexcept {
        return std::clamp(value, min, max);
    }

    /** @brief Returns the maximum of three values.
     * @param a Primo valore.
     * @param b Secondo valore.
     * @param c Terzo valore.
     * @return Il valore massimo tra i tre.
     */
    template <std::totally_ordered T>
    [[nodiscard]] constexpr T max3(const T& a, const T& b, const T& c) noexcept {
        return std::max({ a, b, c });
    }

} // namespace utils