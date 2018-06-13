//
// Created by Nathan Touroux on 08/10/2017.
//

#ifndef HASHLIFE_HASHABLE_HPP
#define HASHLIFE_HASHABLE_HPP

#include <functional>

inline void hash_combine(std::size_t& seed) { }

template <typename T, typename... Rest>
inline void hash_combine(std::size_t& seed, const T& v, Rest... rest) {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
    hash_combine(seed, rest...);
}

#define MAKE_HASHABLE(type, ...) \
    namespace std {\
        template<> struct hash<type> {\
            std::size_t operator()(const type &t) const {\
                std::size_t ret = 0;\
                hash_combine(ret, __VA_ARGS__);\
                return ret;\
            }\
        };\
    }

#define MAKE_HASHABLE_T(type, ...) \
    namespace std {\
        template<class C> struct hash<type<C>> {\
            std::size_t operator()(const type<C> &t) const {\
                std::size_t ret = 0;\
                hash_combine(ret, __VA_ARGS__);\
                return ret;\
            }\
        };\
    }

#define MAKE_HASHABLE_TT(type, ...) \
    namespace std {\
        template<class C, class D> struct hash<type<C,D>> {\
            std::size_t operator()(const type<C,D> &t) const {\
                std::size_t ret = 0;\
                hash_combine(ret, __VA_ARGS__);\
                return ret;\
            }\
        };\
    }

#endif //HASHLIFE_HASHABLE_HPP
