//
// Created by Gregory Postnikov on 2019-07-18.
//

#ifndef TIMER_CGROUP_H
#define TIMER_CGROUP_H

//______________________________________________________________________________________________________________________
class CGroup {
private:

    const std::string name_;
    const size_t students_number_;
    const int64_t available_time_;
    int64_t current_available_time_;

    int cashed_current_available_time_size_;

public:

    CGroup( std::string name,
            size_t students_number,
            int64_t current_available_time );

    const std::string& GetName() const;
    size_t GetStudentsNumber() const;
    int64_t GetAvailableTime() const;
    size_t GetAvailableTimeSize() const;

    void ReserveTime(size_t start_time, size_t duration);
    void ReleaseTime(size_t start_time, size_t duration);
    // Восстановить группу, то есть вернуть к первоначальному состояние свободное время
    void RecoverGroup();

};


#endif //TIMER_CGROUP_H
