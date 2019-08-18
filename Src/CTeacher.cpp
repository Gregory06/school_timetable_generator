//
// Created by Gregory Postnikov on 2019-07-17.
//

#include <cmath>
#include "CTeacher.h"
#include "Defines.h"

//______________________________________________________________________________________________________________________
//______________________________________________________________________________________________________________________
//
// CTeacher
//
//______________________________________________________________________________________________________________________
//______________________________________________________________________________________________________________________

CTeacher::CTeacher( std::string name,
                    int64_t available_time,
                    std::vector<size_t>& time_rating )

                    : name_(name),
                    available_time_(available_time),
                    current_available_time_(available_time),
                    time_rating_(std::move(time_rating)),
                    cashed_current_available_time_size_(NONCASHED)
                    {}

//______________________________________________________________________________________________________________________
// ГЕТТЕРЫ
//______________________________________________________________________________________________________________________

std::string CTeacher::GetName() const {
    return name_;
}

int64_t CTeacher::GetAvailableTime() const {
    return current_available_time_;
}

size_t CTeacher::GetAvailableTimeSize() const {
    if (cashed_current_available_time_size_ >= 0)
        return static_cast<size_t>(cashed_current_available_time_size_);

    size_t counter(0);
    for (int i = 0; i < INT64_SIZE; i++)
        if ( (current_available_time_ >> i) & 1 )
            counter++;

    return counter;
}

//______________________________________________________________________________________________________________________
// МЕТОДЫ  ДЛЯ  РАБОТЫ  С  ЗАНИМАЕМЫМ  И  ОСВОБОЖДАЕМЫМ  ВРЕМЕНЕМ
//______________________________________________________________________________________________________________________

// cм. Cabinet::ReserveTime
void CTeacher::ReserveTime(size_t start_time, size_t duration) {
    int64_t reserved_time_mask = ~( (static_cast<int64_t>( std::pow(2, duration) ) - 1) << start_time );

    current_available_time_ &= reserved_time_mask;

    cashed_current_available_time_size_ = NONCASHED;
}

// cм. Cabinet::ReleaseTime
void CTeacher::ReleaseTime(size_t start_time, size_t duration) {
    int64_t reserved_time_mask = (static_cast<int64_t>( std::pow(2, duration) ) - 1) << start_time;

    current_available_time_ |= reserved_time_mask;

    cashed_current_available_time_size_ = NONCASHED;
}

void CTeacher::RecoverTeacher() {
    current_available_time_ = available_time_;

    cashed_current_available_time_size_ = NONCASHED;
}