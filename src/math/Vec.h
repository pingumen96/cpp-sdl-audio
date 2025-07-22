#pragma once
// math/vec.hpp – n-dimensional constexpr vector
#include <array>
#include <cmath>
#include <concepts>
#include <iostream>
#include <type_traits>
#include <cassert>
#include <algorithm>
#include <functional>

namespace math {

    // --- concept ---------------------------------------------------------------
    template <typename T>
    concept Arithmetic = std::is_arithmetic_v<T>;

    // --- Vec<N,T> --------------------------------------------------------------
    template <std::size_t N, Arithmetic T>
    struct Vec {
        using ValueType = T;
        static constexpr std::size_t Dim = N;

        std::array<T, N> data{};

        /*---------------- constructors ----------------*/
        /**
         * @brief Costruttore di default. Inizializza tutti gli elementi a zero.
         */
        constexpr Vec() noexcept = default;

        /**
         * @brief Costruttore che accetta N valori convertibili a T.
         * @tparam U Tipi dei valori da convertire.
         * @param xs Valori da assegnare agli elementi del vettore.
         */
        template <class... U>
            requires (sizeof...(U) == N) &&
        (std::is_convertible_v<U, T> && ...)
            explicit constexpr Vec(U... xs) noexcept : data{ static_cast<T>(xs)... } {}

        /**
         * @brief Costruttore di conversione da altro tipo aritmetico.
         * @tparam U Tipo aritmetico sorgente.
         * @param other Vettore da convertire.
         */
        template <Arithmetic U>
        constexpr Vec(const Vec<N, U>& other) noexcept {
            for (std::size_t i = 0; i < N; ++i)
                data[i] = static_cast<T>(other[i]);
        }

        /*---------------- indexing ----------------*/
        /**
         * @brief Accesso in scrittura all'i-esimo elemento.
         * @param i Indice dell'elemento.
         * @return Riferimento all'elemento.
         */
        constexpr       T& operator[](std::size_t i)       noexcept { assert(i < N); return data[i]; }
        /**
         * @brief Accesso in sola lettura all'i-esimo elemento.
         * @param i Indice dell'elemento.
         * @return Riferimento costante all'elemento.
         */
        constexpr const T& operator[](std::size_t i) const noexcept { assert(i < N); return data[i]; }

        /*---------------- arithmetic ----------------*/
        /**
         * @brief Somma un altro vettore a questo (component-wise).
         * @param o Vettore da sommare.
         * @return Riferimento a *this.
         */
        [[nodiscard]] constexpr Vec& operator+=(const Vec& o) noexcept {
            for (std::size_t i = 0; i < N; ++i) data[i] += o[i];
            return *this;
        }
        /**
         * @brief Sottrae un altro vettore da questo (component-wise).
         * @param o Vettore da sottrarre.
         * @return Riferimento a *this.
         */
        [[nodiscard]] constexpr Vec& operator-=(const Vec& o) noexcept {
            for (std::size_t i = 0; i < N; ++i) data[i] -= o[i];
            return *this;
        }
        /**
         * @brief Moltiplica ogni elemento per uno scalare.
         * @param s Scalare.
         * @return Riferimento a *this.
         */
        [[nodiscard]] constexpr Vec& operator*=(T s) noexcept {
            for (auto& v : data) v *= s;
            return *this;
        }
        /**
         * @brief Divide ogni elemento per uno scalare.
         * @param s Scalare divisore.
         * @return Riferimento a *this.
         * @throws std::domain_error se s == 0.
         */
        [[nodiscard]] constexpr Vec& operator/=(T s) {
            if (s == T{}) throw std::domain_error("Division by zero");
            for (auto& v : data) v /= s;
            return *this;
        }

        /*---------------- unary ----------------*/
        /**
         * @brief Operatore unario + (identità).
         * @return Una copia del vettore.
         */
        [[nodiscard]] constexpr Vec  operator+() const noexcept { return *this; }
        /**
         * @brief Operatore unario - (opposto).
         * @return Vettore opposto.
         */
        [[nodiscard]] constexpr Vec  operator-() const noexcept {
            Vec r{};
            for (std::size_t i = 0; i < N; ++i) r[i] = -data[i];
            return r;
        }

        /*---------------- binary free ops ----------------*/
        /**
         * @brief Somma due vettori (component-wise).
         * @param a Primo vettore.
         * @param b Secondo vettore.
         * @return Somma dei due vettori.
         */
        friend constexpr Vec operator+(Vec a, const Vec& b) noexcept { return a += b; }
        /**
         * @brief Sottrae due vettori (component-wise).
         * @param a Primo vettore.
         * @param b Secondo vettore.
         * @return Differenza dei due vettori.
         */
        friend constexpr Vec operator-(Vec a, const Vec& b) noexcept { return a -= b; }
        /**
         * @brief Moltiplica un vettore per uno scalare.
         * @param a Vettore.
         * @param s Scalare.
         * @return Vettore risultante.
         */
        friend constexpr Vec operator*(Vec a, T s)        noexcept { return a *= s; }
        /**
         * @brief Moltiplica uno scalare per un vettore.
         * @param s Scalare.
         * @param a Vettore.
         * @return Vettore risultante.
         */
        friend constexpr Vec operator*(T s, Vec a)        noexcept { return a *= s; }
        /**
         * @brief Divide un vettore per uno scalare.
         * @param a Vettore.
         * @param s Scalare divisore.
         * @return Vettore risultante.
         */
        friend constexpr Vec operator/(Vec a, T s) { return a /= s; }

        /**
         * @brief Confronta due vettori per uguaglianza.
         */
        friend constexpr bool operator==(const Vec&, const Vec&) = default;
        /**
         * @brief Operatore di confronto a tre vie (spaceship).
         */
        friend constexpr auto operator<=>(const Vec&, const Vec&) = default;

        /*---------------- math helpers ----------------*/
        /**
         * @brief Prodotto scalare tra due vettori.
         * @param o Altro vettore.
         * @return Prodotto scalare.
         */
        [[nodiscard]] constexpr T dot(const Vec& o) const noexcept {
            T acc{};
            for (std::size_t i = 0; i < N; ++i) acc += data[i] * o[i];
            return acc;
        }

        /**
         * @brief Quadrato della lunghezza del vettore.
         * @return Somma dei quadrati degli elementi.
         */
        [[nodiscard]] constexpr auto lengthSquared() const noexcept { return dot(*this); }

        /**
         * @brief Lunghezza (norma) del vettore.
         * @return Norma euclidea.
         */
        [[nodiscard]] constexpr auto length() const noexcept {
            return std::sqrt(static_cast<std::common_type_t<T, double>>(lengthSquared()));
        }

        /**
         * @brief Restituisce il vettore normalizzato (lunghezza 1).
         * @return Vettore normalizzato.
         * @throws assert se la lunghezza è zero.
         */
        [[nodiscard]] constexpr Vec normalized() const {
            auto len = length();
            assert(len != T{} && "Cannot normalize zero-length vector");
            return (*this) / static_cast<T>(len);
        }

        /*---------------- interpolation ----------------*/
        /**
         * @brief Interpolazione lineare tra due vettori.
         * t = 0 → a, t = 1 → b
         * @tparam U Qualsiasi tipo aritmetico convertibile a common_type<T,U>
         * @param a Vettore di partenza.
         * @param b Vettore di arrivo.
         * @param t Fattore di blending (0–1 di solito, ma non obbligatorio).
         * @return Vettore interpolato.
         */
        template <Arithmetic U>
        [[nodiscard]] static constexpr Vec lerp(const Vec& a,
            const Vec& b,
            U t) noexcept {
            using CT = std::common_type_t<T, U>;   // calcoli in precisione “comune”
            Vec r{};
            const CT ct = static_cast<CT>(t);

            for (std::size_t i = 0; i < N; ++i) {
                const CT ai = static_cast<CT>(a[i]);
                const CT bi = static_cast<CT>(b[i]);
                r[i] = static_cast<T>(ai + (bi - ai) * ct);
            }
            return r;
        }


        /*---------------- factories ----------------*/
        /**
         * @brief Restituisce un vettore con tutti gli elementi a zero.
         * @return Vettore nullo.
         */
        [[nodiscard]] static constexpr Vec zero() noexcept { return {}; }

        /**
         * @brief Restituisce un vettore con tutti gli elementi a uno.
         * @return Vettore con tutti 1.
         */
        [[nodiscard]] static constexpr Vec one() noexcept {
            Vec r{};
            r.data.fill(T{ 1 });
            return r;
        }

        /*---------------- ostream ----------------*/
        /**
         * @brief Stampa il vettore su uno stream di output.
         * @param os Stream di output.
         * @param v Vettore da stampare.
         * @return Riferimento allo stream.
         */
        friend std::ostream& operator<<(std::ostream& os, const Vec& v) {
            os << "Vec" << N << "(";
            for (std::size_t i = 0; i < N; ++i) {
                os << v[i] << (i + 1 < N ? ", " : "");
            }
            return os << ")";
        }
    };

    /*================ special utilities =================*/
    /* --- cross product --- */
    template <Arithmetic T>
    /**
     * @brief Prodotto vettoriale tra due vettori 2D.
     * @param a Primo vettore.
     * @param b Secondo vettore.
     * @return Risultato dello pseudo-prodotto vettoriale.
     */
    [[nodiscard]] constexpr T cross(const Vec<2, T>& a, const Vec<2, T>& b) noexcept {
        return a[0] * b[1] - a[1] * b[0];
    }

    template <Arithmetic T>
    /**
     * @brief Prodotto vettoriale tra due vettori 3D.
     * @param a Primo vettore.
     * @param b Secondo vettore.
     * @return Vettore risultante dal prodotto vettoriale.
     */
    [[nodiscard]] constexpr Vec<3, T> cross(const Vec<3, T>& a, const Vec<3, T>& b) noexcept {
        return {
            a[1] * b[2] - a[2] * b[1],
            a[2] * b[0] - a[0] * b[2],
            a[0] * b[1] - a[1] * b[0]
        };
    }

    /* --- clamp (component-wise) --- */
    template <std::size_t N, Arithmetic T>
    /**
     * @brief Clampa ogni componente del vettore tra min e max (component-wise).
     * @param v Vettore da clampare.
     * @param min Limite inferiore.
     * @param max Limite superiore.
     * @return Vettore clampato.
     */
    [[nodiscard]] constexpr Vec<N, T> clamp(const Vec<N, T>& v,
        const Vec<N, T>& min,
        const Vec<N, T>& max) noexcept {
        Vec<N, T> r{};
        for (std::size_t i = 0; i < N; ++i)
            r[i] = std::clamp(v[i], min[i], max[i]);
        return r;
    }

    /*================ global utility functions =================*/

    template <std::size_t N, Arithmetic T>
    /**
     * @brief Prodotto scalare globale tra due vettori.
     */
    [[nodiscard]] constexpr T dot(const Vec<N, T>& a, const Vec<N, T>& b) noexcept {
        return a.dot(b);
    }

    template <std::size_t N, Arithmetic T>
    /**
     * @brief Normalizza un vettore.
     */
    [[nodiscard]] constexpr Vec<N, T> normalize(const Vec<N, T>& v) {
        return v.normalized();
    }

    /*================ handy aliases =================*/
    template <Arithmetic T> using Vec2 = Vec<2, T>;
    template <Arithmetic T> using Vec3 = Vec<3, T>;
    template <Arithmetic T> using Vec4 = Vec<4, T>;

    using Vec2f = Vec2<float>;
    using Vec3f = Vec3<float>;
    using Vec4f = Vec4<float>;

    using Vec2d = Vec2<double>;
    using Vec3d = Vec3<double>;
    using Vec4d = Vec4<double>;

    using Vec2i = Vec2<int>;
    using Vec3i = Vec3<int>;
    using Vec4i = Vec4<int>;

} // namespace math

/*================ hash specialisation =================*/
namespace std {
    template <size_t N, math::Arithmetic T>
    /**
     * @brief Specializzazione di std::hash per math::Vec.
     */
    struct hash<math::Vec<N, T>> {
        size_t operator()(const math::Vec<N, T>& v) const noexcept {
            size_t seed = 0x9e3779b97f4a7c15ULL;
            for (size_t i = 0; i < N; ++i) {
                size_t h = hash<T>{}(v[i]);
                seed ^= h + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
            }
            return seed;
        }
    };
} // namespace std
