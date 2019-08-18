//
// Created by Gregory Postnikov on 2019-07-20.
//

#ifndef TIMER_CCABINET_H
#define TIMER_CCABINET_H

//______________________________________________________________________________________________________________________
class CCabinet {
private:

    const std::string name_;
    const size_t capacity_;
    const int64_t available_time_;

    int64_t current_available_time_;

public:

    CCabinet( std::string name,
              size_t capacity,
              int64_t available_time );

    CCabinet ( const CCabinet& other ) = default;

    std::string GetName() const;
    int64_t GetAvailableTime() const;
    size_t GetCapacity() const;
    bool IsFeasible(size_t start_time, size_t duration) const;

    void ReserveTime(size_t start_time, size_t duration);
    void ReleaseTime(size_t start_time, size_t duration);
    // Восстановить кабинет, то есть вернуть свободное время к начальному состоянию
    void RecoverCabinet();

};


#endif //TIMER_CCABINET_H
