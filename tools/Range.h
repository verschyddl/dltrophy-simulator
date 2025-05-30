//
// Created by qm210 on 30.05.2025.
//

#ifndef DLTROPHY_SIMULATOR_RANGE_H
#define DLTROPHY_SIMULATOR_RANGE_H

namespace Range {


    std::vector<int> vectorFrom(std::ranges::iota_view<int> source) {
        std::vector<int> result;
        std::ranges::copy(source, std::back_inserter(result));
        return result;
    }

    template <std::ranges::range R>
    requires std::same_as<std::ranges::range_value_t<R>, int>
    std::vector<int> vectorFrom(R&& source) {
        std::vector<int> result;
        std::ranges::copy(source, std::back_inserter(result));
        return result;
    }

    std::vector<int> range(int start, int count) {
        return vectorFrom(
                std::ranges::iota_view{start, start + count}
        );
    }

    std::vector<int> range(int start, int count, int step) {
        return vectorFrom(
                std::ranges::iota_view{0, count}
                | std::views::transform([start, step](int x) {
                    return start + step * x;
                })
        );
    }


}

#endif //DLTROPHY_SIMULATOR_RANGE_H
