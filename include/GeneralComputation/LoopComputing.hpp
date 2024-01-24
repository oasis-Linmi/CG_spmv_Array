#pragma once
#include <array>
#include <cstddef>
#include <iostream>
#include <utility>
namespace ZCCTools {
/*constexpr Loop*/
template <class Lam, size_t... Is>
[[gnu::always_inline]] constexpr void Loop(Lam lam,
                                           std::index_sequence<Is...>) {
    (lam(Is), ...);
};

/*constexpr Loop*/
template <size_t Dim, class Lam>
[[gnu::always_inline]] void Loop(Lam lam) {
    Loop([&](auto... Is) [[gnu::always_inline]] { lam(Is...); },
         std::make_index_sequence<Dim>{});
}

/*the deep loop used to implememt nested loop*/
template <class Lam, size_t Dim, class... ARRAY>
[[gnu::always_inline]] void DeepLoop(Lam lam,
                                     const std::array<size_t, Dim>& start_l,
                                     const std::array<size_t, Dim>& end_l,
                                     ARRAY&&... arrays) {
    if constexpr (Dim == 1) {
        for (int i = start_l[0]; i < end_l[0]; i++) {
            lam(i, std::forward<decltype(arrays)>(arrays)...);
        }
    } else {
        std::array<size_t, Dim - 1> start_r;
        std::array<size_t, Dim - 1> end_r;
        Loop<Dim - 1>([&](auto i) [[gnu::always_inline]] {
            start_r[i] = start_l[i + 1];
            end_r[i] = end_l[i + 1];
        });
        for (int i = start_l[0]; i < end_l[0]; i++) {
            auto lam_new = [i, lam] [[gnu::always_inline]] (auto&&... args) {
                lam(i, std::forward<decltype(args)>(args)...);
            };
            DeepLoop(lam_new, start_r, end_r,
                     std::forward<decltype(arrays)>(arrays)...);
        }
    }
}
}  // namespace ZCCTools