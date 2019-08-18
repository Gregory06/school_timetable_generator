//
// Created by Gregory Postnikov on 2019-07-20.
//

#include "CEvent.h"

//______________________________________________________________________________________________________________________
//______________________________________________________________________________________________________________________
//
// CEvent
//
//______________________________________________________________________________________________________________________
//______________________________________________________________________________________________________________________

CEvent::CEvent()
    : subject_(nullptr),
    cabinets_{},
    start_time_(0),
    myself_()
    {}

bool CEvent::operator==(const CEvent &other) const {
    return other.GetSubject() == GetSubject();
}

//______________________________________________________________________________________________________________________
// ГЕТТЕРЫ
//______________________________________________________________________________________________________________________

std::string CEvent::GetName() const {
    if ( !subject_ )
        return "none";
    return subject_->GetName();
}

size_t CEvent::GetId() const {
    return subject_->GetId();
}

CSubject* CEvent::GetSubject() const {
    return subject_;
}

size_t CEvent::GetStartTime() const {
    return start_time_;
}

const std::set<CTeacher *const, Comparator<CTeacher>>& CEvent::GetTeachers() const {
    return subject_->GetTeachers();
}

const std::set<CCabinet *const, Comparator<CCabinet>>& CEvent::GetCabinets() const {
    return cabinets_;
}

bool CEvent::IsActive() const {
    return subject_ != nullptr;
}

//______________________________________________________________________________________________________________________
// СЕТТЕРЫ
//______________________________________________________________________________________________________________________

void CEvent::SetSubject(const CSubject* subject) {
    subject_ = const_cast<CSubject*>(subject);
}

void CEvent::SetCabinets(std::set<CCabinet *const, Comparator<CCabinet>> cabinet) {
    cabinets_ = std::move(cabinet);
}

void CEvent::SetStartTime(size_t start_time) {
    start_time_ = start_time;
}

//______________________________________________________________________________________________________________________
// МОДИФИКАТОРЫ
//______________________________________________________________________________________________________________________

void CEvent::FreeEvent() {
    subject_ = nullptr;
    cabinets_.clear();
    start_time_ = 0;
}

//______________________________________________________________________________________________________________________
//______________________________________________________________________________________________________________________
//
// CEvent
//
//______________________________________________________________________________________________________________________
//______________________________________________________________________________________________________________________

CEventLinker::CEventLinker( const std::map<std::string, CSubject> &subjects_,
                            const std::map<std::string, CGroup> &groups_ ) {
    for ( const auto& [group_name, group] : groups_) {
        if ( linked_events_.find(group_name) == linked_events_.end() )
            linked_events_.insert( std::make_pair(group_name, std::map<size_t, std::set<CEvent *, EventComporator> >{}) );
        for (const auto& [subject_name, subject] : subjects_)
            if ( linked_events_.at(group_name).find(subject.GetId()) == linked_events_.at(group_name).end() )
                linked_events_.at(group_name).insert(std::make_pair(subject.GetId(), std::set<CEvent *, EventComporator>{}));
    }
}

//______________________________________________________________________________________________________________________
// ГЕТТЕРЫ
//______________________________________________________________________________________________________________________

const auto& CEventLinker::GetLinkedEvents() const {
    return linked_events_;
}

//______________________________________________________________________________________________________________________
// МОДИФИКАТОРЫ
//______________________________________________________________________________________________________________________

void CEventLinker::InsertEvent(std::string group_name, CEvent& event) {
    event.myself_ = linked_events_.at( group_name ).at( event.GetId() ).insert(&event).first;
}

void CEventLinker::DeleteEvent(std::string group_name, CEvent& event) {
    linked_events_.at( group_name ).at( event.GetId() ).erase( event.myself_ );
}

void CEventLinker::FreeEvents() {
    for(auto& [group_name, lists] : linked_events_)
        for(auto& [id, events] : lists)
            events.clear();
}
