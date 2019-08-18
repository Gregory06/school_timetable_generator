//
// Created by Gregory Postnikov on 2019-07-17.
//

#ifndef TIMER_CTEACHER_H
#define TIMER_CTEACHER_H

#include <vector>

class CTeacher {
private:

    const std::string name_;
    const int64_t available_time_;
    const std::vector<size_t> time_rating_;
    int64_t current_available_time_;

    int cashed_current_available_time_size_;

public:

    CTeacher( std::string name,
              int64_t avalaible_time,
              std::vector<size_t>& time_rating );

    CTeacher( const CTeacher& other ) = default;

    std::string GetName() const;
    int64_t GetAvailableTime() const;
    size_t GetAvailableTimeSize() const;

    void ReserveTime(size_t start_time, size_t duration);
    void ReleaseTime(size_t start_time, size_t duration);
    void RecoverTeacher();

};


#endif //TIMER_CTEACHER_H
