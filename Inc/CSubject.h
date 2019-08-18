//
// Created by Gregory Postnikov on 2019-07-18.
//

#ifndef TIMER_CSUBJECT_H
#define TIMER_CSUBJECT_H

#pragma once

#include <vector>
#include <unordered_set>
#include <set>
#include <map>
#include "CTeacher.h"
#include "CGroup.h"
#include "ServiceFunctions.h"

//______________________________________________________________________________________________________________________
class CSubject {
private:

    // Конструктор недоступен. Для создания используется
    // вспомогательный класс CSubjectBuilder ( метод Build )
    CSubject( std::string name,
              size_t id,
              size_t difficulty_rating,
              size_t duration,
              size_t required_cabinets_number,
              int64_t feasible_time_,
              const std::set<CTeacher *const, Comparator<CTeacher>>& teachers,
              const std::set<CGroup *const, Comparator<CGroup>>& groups,
              const std::set<CCabinet *const, Comparator<CCabinet>>& cabinets,
              size_t total_participants );

    const std::string name_;
    const size_t id_;
    const size_t difficulty_rating_;
    const size_t duration_;
    const size_t required_cabinets_number_;
    const int64_t feasible_time_;
    const std::set<CTeacher *const, Comparator<CTeacher>> teachers_;
    const std::set<CGroup *const, Comparator<CGroup>> groups_;
    const std::set<CCabinet *const, Comparator<CCabinet>> cabinets_;
    const size_t total_participants_;

public:

    std::string GetName() const;
    size_t GetId() const;
    size_t GetDuration() const;
    size_t GetRequiredCabinetsNumber() const;
    size_t GetParticipantsNumber() const;

    const auto& GetGroups() const;
    const auto& GetTeachers() const;
    const auto& GetCabinets() const;
    int64_t GetAvailableTime() const;

    int64_t GetFeasibleTime() const;
    int64_t GetAvailableStartTime( size_t days_in_week, size_t lessons_in_day ) const;
    int64_t GetGroupAvailableTime() const;
    int64_t GetTeachersAvailableTime() const;
    size_t GetTeachersAvailableTimeSize() const;

    void ReserveTime(size_t start_time);
    void ReleaseTime(size_t start_time);

    friend class CSubjectBuilder;

};

// Вспомогательный класс для удобного создания объектов CSubject
// Содержит набор необходимых сеттеров, после вызовов которых объект CSubject
// создается вызовом метода Build.
//______________________________________________________________________________________________________________________
class CSubjectBuilder {
private:

    std::string name_;
    size_t id_;
    size_t difficulty_rating_;
    size_t duration_;
    size_t required_cabinets_number_;
    int64_t feasible_time_;
    std::set<CTeacher *const, Comparator<CTeacher>> teachers_;
    std::set<CGroup *const, Comparator<CGroup>> groups_;
    std::set<CCabinet *const, Comparator<CCabinet>> cabinets_;
    size_t total_participants_;

public:

    void SetSubjectName(std::string name);
    void SetSubjectId(size_t id);
    void SetSubjectDifficultyRating(size_t difficulty_rating);
    void SetSubjectDuration(size_t duration);
    void SetRequiredCabinetNumber(size_t required_cabinets_number);
    void SetFeasibleTime(int64_t feasible_time);
    void SetSubjectTeachers(std::set<CTeacher *const, Comparator<CTeacher>> teachers);
    void SetSubjectGroups(std::set<CGroup *const, Comparator<CGroup>> groups);
    void SetSubjectCabinets(std::set<CCabinet *const, Comparator<CCabinet>> cabinets);

    CSubject Build() const;

};

// Отношение порядка на множестве предметов -- мощность множества доступных времен начала
//______________________________________________________________________________________________________________________
struct SubjectComporator {
    bool operator() (const CSubject* a, const CSubject* b) const {
        return a->GetTeachersAvailableTimeSize() < b->GetTeachersAvailableTimeSize();
    }
};

// Вспомогательный класс для хранения предметов при генерации расписания.
// Поддерживает очередь с приоритетом предметов для расмещения в расписании, стек размещенных предметов.
// стек времени размещенных предметов. При генерации расписания предметы выбираются из очереди и перемещаются в стек.
// Если очередной предмет разместить не получается, происходит откат ( BackTrack ).
//______________________________________________________________________________________________________________________
class CTimeTableGeneratorSupporter {
private:

    std::vector<CSubject*> stack_;
    std::multiset<CSubject *const, SubjectComporator> priority_queue_;
    std::vector< std::vector<size_t> > times_stack_;

    size_t days_in_week_, lessons_in_day_;
    bool is_last_successful_;

    // Переместить в очередь предмет с вершины стека
    void moveTopToQueue();
    // Push первого по приоритету элемента очереди в стек
    void moveMinToStack();

    // Произвести откат, занеся в вектор пары ( премет, время начала ), которые нужно удалить из расписания
    void backTrack(std::vector< std::pair<CSubject *, size_t> >& subjects_to_delete);

public:

    CTimeTableGeneratorSupporter( std::map< std::string,CSubject >& subjects,
                                  size_t days_in_week, size_t lessons_in_day );

    // Совершить очередную итерацию при генерации: или перенос из очереди в стек, если предыдущая итерация прошла
    // успешно, или совершить откат
    void MakeIteration(std::vector< std::pair<CSubject *, size_t> >& subjects_to_delete);
    // Поставить флаг, обозначающий неудачно выполненную итерацию: is_last_successful_ = false
    void SetFailureFlag();
    // Удачна ли предыдущая итерация
    bool IsLastSuccessful() const;

    // stack_pop стека времени и остправить предмет с вершины стека предметов в очередь
    void CleanStackTop();
    void TimesStackPop();

    bool QueueEmpty() const;
    bool StackEmpty() const;
    bool CurrentSubjectTimesStackEmpty() const;

    // Получить элемент с вершины стека предметов, т.е. текущий для размещения
    CSubject *const GetCurrentSubject() const;
    size_t GetCurrentSubjectStartTime() const;

};

#endif //TIMER_CSUBJECT_H
