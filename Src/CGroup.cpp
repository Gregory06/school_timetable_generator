//
// Created by Gregory Postnikov on 2019-07-18.
//

#include <cmath>
#include "CGroup.h"

//______________________________________________________________________________________________________________________
//______________________________________________________________________________________________________________________
//
// CGroup
//
//______________________________________________________________________________________________________________________
//______________________________________________________________________________________________________________________

CGroup::CGroup( const std::string name,
                const size_t students_number,
                int64_t available_time )

                : name_(name),
                students_number_(students_number),
                available_time_(available_time),
                current_available_time_(available_time)
                {}

//______________________________________________________________________________________________________________________
// ГЕТТЕРЫ
//______________________________________________________________________________________________________________________

const std::string& CGroup::GetName() const {
    return name_;
}

size_t CGroup::GetStudentsNumber() const {
    return students_number_;
}

int64_t CGroup::GetAvailableTime() const {
    return current_available_time_;
}

size_t CGroup::GetAvailableTimeSize() const {
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

// см. CCabinet::ReserveTime
void CGroup::ReserveTime(size_t start_time, size_t duration) {
    int64_t reserved_time_mask = ~( (static_cast<int64_t>( std::pow(2, duration) ) - 1) << start_time );

    current_available_time_ &= reserved_time_mask;
}

// см. CCabinet::ReleaseTime
void CGroup::ReleaseTime(size_t start_time, size_t duration) {
    int64_t reserved_time_mask = (static_cast<int64_t>( std::pow(2, duration) ) - 1) << start_time;

    current_available_time_ |= reserved_time_mask;
}

void CGroup::RecoverGroup() {
    current_available_time_ = available_time_;
}