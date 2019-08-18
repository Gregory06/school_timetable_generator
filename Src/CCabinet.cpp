//
// Created by Gregory Postnikov on 2019-07-20.
//

#include "CCabinet.h"

//______________________________________________________________________________________________________________________
//______________________________________________________________________________________________________________________
//
// CCabinet
//
//______________________________________________________________________________________________________________________
//______________________________________________________________________________________________________________________

CCabinet::CCabinet( std::string name,
                    size_t capacity,
                    int64_t available_time )
                    : name_(name),
                    capacity_(capacity),
                    available_time_(available_time),
                    current_available_time_(available_time)
                    {}

//______________________________________________________________________________________________________________________
// ГЕТТЕРЫ
//______________________________________________________________________________________________________________________

std::string CCabinet::GetName() const {
    return name_;
}

int64_t CCabinet::GetAvailableTime() const {
    return current_available_time_;
}

size_t CCabinet::GetCapacity() const {
    return capacity_;
}

//______________________________________________________________________________________________________________________
// МЕТОДЫ  ДЛЯ  РАБОТЫ  С  ЗАНИМАЕМЫМ  И  ОСВОБОЖДАЕМЫМ  ВРЕМЕНЕМ
//______________________________________________________________________________________________________________________

bool CCabinet::IsFeasible(size_t start_time, size_t duration) const {
    // Создаем маску времени: duration единиц, начиная с бита start_time+1, считая началом
    // младшие биты. Например, для start_time = 5 и duration 2 получим:
    // 0000000000000000000000000000000000000000000000000000000001100000.
    // Сначала создаем duration единиц, начиная с нулевого бита:
    // это просто сумма duration первых членов геметрической прогрессии со знаменателем 2.
    // Далее, сдвигаем получившееся на start_time битов влево.
    int64_t time_mask = (static_cast<int64_t>( std::pow(2, duration) ) - 1) << start_time ;

    // Если кабинет действительно доступен на duration со start_time, значит в current_available_time_
    // под соответсвующей этим параметрам маской стоят единицы. Тогда логическое И превратит
    // time_mask & current_available_time_ в time_mask. Если же кабинет недоступен, тогда хотя бы на 1 месте под маской
    // стоит 0, и !( (time_mask & current_available_time_) == time_mask )
    return (time_mask & current_available_time_) == time_mask;
}

void CCabinet::ReserveTime(size_t start_time, size_t duration) {
    // Делаем маску времени, как в CCabinet::IsFeasible и инвертируем. Теперь на месте нулей стоят единицы и наоборот.
    int64_t reserved_time_mask = ~( (static_cast<int64_t>( std::pow(2, duration) ) - 1) << start_time );

    // Применив логическое И, добъемся того, что на битах, соответсвующих занимаемому времени теперь будут находиться
    // нули, что и значит, что время занято.
    current_available_time_ &= reserved_time_mask;
}

void CCabinet::ReleaseTime(size_t start_time, size_t duration) {
    // Делаем маску времени, как в CCabinet::IsFeasible
    int64_t released_time_mask = (static_cast<int64_t>( std::pow(2, duration) ) - 1) << start_time;

    // Применив логическое ИЛИ, добъемся того, что на битах, соответсвующих освобождаемому времени теперь будут
    // находиться удиницы, что и значит, что время свободно.
    current_available_time_ |= released_time_mask;
}

void CCabinet::RecoverCabinet() {
    current_available_time_ = available_time_;
}