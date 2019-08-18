//
// Created by Gregory Postnikov on 2019-07-20.
//

#include <vector>
#include <random>
#include <algorithm>

#ifndef TIMER_SERVICEFUNCTIONS_H
#define TIMER_SERVICEFUNCTIONS_H

std::vector<size_t> RandomPermutation(int64_t time) {
    std::vector<size_t> result;
    result.reserve(sizeof(long long) * 8);

    for ( int i = 0; i < sizeof(long long) * 8; i++ )
        if (time & (static_cast<int64_t>(1) << i))
            result.push_back(i);

    std::random_device rd;
    std::mt19937 g(rd());

    std::shuffle(result.begin(), result.end(), g);

    return std::move(result);

}

template <typename T>
void RandomPermutation(std::vector<T>& vec) {

    std::random_device rd;
    std::mt19937 g(rd());

    std::shuffle(vec.begin(), vec.end(), g);

}

template <class T>
struct Comparator {
    bool operator() (T *const a, T *const b) const {
        return a->GetName() < b->GetName();
    }
    bool operator() (const T *const a, const T *const b) const {
        return a->GetName() < b->GetName();
    }
};

int64_t Str2Int64(std::string str) {
    int64_t result(0);

    for (int i = 0; i < str.size(); i++) {
        if (str[i] == '1')
            result |= (static_cast<int64_t>(1) << (str.size()-1 - i));
    }

    return result;
}

std::string Int642Str(int64_t int64) {
    std::string result;

    for (int i = 63; i > -1; i--) {
        if ( int64 & (static_cast<int64_t>(1) << i) )
            result += "1";
        else
            result += "0";
    }

    return result;
}

#endif //TIMER_SERVICEFUNCTIONS_H
