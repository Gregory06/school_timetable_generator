//
// Created by Gregory Postnikov on 2019-07-20.
//

#ifndef TIMER_CEVENT_H
#define TIMER_CEVENT_H

#include "CSubject.h"
#include "CCabinet.h"
#include <list>
#include <map>

// Событие -- класс, представляющий предмет в "пространстве-времени", т.е. предмет с кабинетом и временем начала
//______________________________________________________________________________________________________________________
class CEvent {
private:

    CSubject* subject_;
    std::set<CCabinet *const, Comparator<CCabinet>> cabinets_;
    size_t start_time_;

    // Итератор на себя в CEventLinker для быстрого доступа при удалении
    std::set<CEvent*>::iterator myself_;

    friend class CEventLinker;

public:

    CEvent();
    bool operator==(const CEvent& other) const;


    std::string GetName() const;
    size_t GetId() const;
    CSubject* GetSubject() const;
    size_t GetStartTime() const;
    const std::set<CTeacher *const, Comparator<CTeacher>>& GetTeachers() const;
    const std::set<CCabinet *const, Comparator<CCabinet>>& GetCabinets() const;
    bool IsActive() const;

    void SetSubject(const CSubject* subject);
    void SetCabinets(std::set<CCabinet *const, Comparator<CCabinet>> cabinet);
    void SetStartTime(size_t start_time);

    // Освободить событие, то есть subject_ = nullptr, т.к. активность события -- это subject_ == nullptr
    void FreeEvent();

};

// Отношение порядка на множестве событий -- их положение во времени
//______________________________________________________________________________________________________________________
struct EventComporator {
    bool operator() (const CEvent* a, const CEvent* b) const {
        return a->GetStartTime() < b->GetStartTime();
    }
};

// Вспомогательный класс, служащий для того, чтобы связывать события, представляющие копии одних и тех же предметов,
// т.е. в одним и тем же id в рамках одной группы. Например, если на неделе 10 уроков математики у 11А, то данный класс
// предоставляет возможность итерироваться именно по списку уроков математики в 11А. Основная миссия -- быстрый доступ
// к событиям с одинаковым id при подсчете функции ошибки ( не должно быть скоплений событий, представляющих
// один и тот же предмет )
//______________________________________________________________________________________________________________________
class CEventLinker {
private:

    std::map < std::string, std::map< size_t, std::set<CEvent*, EventComporator> > > linked_events_;

public:

    CEventLinker( const std::map<std::string, CSubject> &subjects_,
                  const std::map<std::string, CGroup> &groups_ );

    const auto& GetLinkedEvents() const;

    void InsertEvent(std::string group_name, CEvent& event);
    void DeleteEvent(std::string group_name, CEvent& event);
    // Очистить все списки предметов, оставив только струткуру, т.е. group_name x id x std::set. Последние во всех
    // тройках будт пусты. Используется при восстановлении CTimeTable.
    void FreeEvents();

};

#endif //TIMER_CEVENT_H
