//
// Created by Gregory Postnikov on 2019-07-18.
//

#include "CSubject.h"
#include "CGroup.h"
#include "ServiceFunctions.h"
#include "CException.h"

//______________________________________________________________________________________________________________________
//______________________________________________________________________________________________________________________
//
// CSubject
//
//______________________________________________________________________________________________________________________
//______________________________________________________________________________________________________________________

CSubject::CSubject( std::string name,
                    size_t id,
                    size_t difficulty_rating,
                    size_t duration,
                    size_t required_cabinets_number,
                    int64_t feasible_time,
                    const std::set<CTeacher *const, Comparator<CTeacher>> &teachers,
                    const std::set<CGroup *const, Comparator<CGroup>> &groups,
                    const std::set<CCabinet *const, Comparator<CCabinet>> &cabinets,
                    size_t total_participants)

                    : name_(name),
                    id_(id),
                    difficulty_rating_(difficulty_rating),
                    duration_(duration),
                    required_cabinets_number_(required_cabinets_number),
                    feasible_time_(feasible_time),
                    teachers_(teachers),
                    groups_(groups),
                    cabinets_(cabinets),
                    total_participants_(total_participants)
                    {}

//______________________________________________________________________________________________________________________
// ГЕТТЕРЫ
//______________________________________________________________________________________________________________________

std::string CSubject::GetName() const {
    return name_;
}

size_t CSubject::GetId() const {
    return id_;
}

size_t CSubject::GetDuration() const {
    return duration_;
}

size_t CSubject::GetRequiredCabinetsNumber() const {
    return required_cabinets_number_;
}

size_t CSubject::GetParticipantsNumber() const {
    size_t participants_number(0);

    for ( const auto& group : groups_ )
        participants_number += group->GetStudentsNumber();

    return participants_number;
}

const auto& CSubject::GetGroups() const {
    return groups_;
}

const auto& CSubject::GetTeachers() const {
    return teachers_;
}

const auto& CSubject::GetCabinets() const {
    return cabinets_;
}

int64_t CSubject::GetAvailableTime() const {
    int64_t resulting_available_time( GetTeachersAvailableTime() & GetGroupAvailableTime() & GetFeasibleTime() );

    return resulting_available_time;
}

int64_t CSubject::GetFeasibleTime() const {
    return feasible_time_;
}

int64_t CSubject::GetAvailableStartTime( size_t days_in_week,
                                         size_t lessons_in_day ) const {

    int64_t resulting_available_start_time( GetAvailableTime() );

    // Маска предмета -- duration_ подряд идущих единиц
    int64_t duration_mask(0);
    for (int i = 0; i < duration_; i++)
        duration_mask |= static_cast<int64_t>(1) << i;

    // Сдвигая duration_mask на day * lessons_in_day + lesson, проверяем, доступен ли предмет на всех местах единиц
    // в duration_mask. Если нет, меняем бит day * lessons_in_day + lesson в resulting_available_start_time на 0, так
    // как с этого времени начать предмет нельзя. Дополнительно зануляем времяв конце дня, куда не уместится предмет.
    for ( int day = 0; day < days_in_week; day++ ) {
        for ( int lesson = 0; lesson < int(lessons_in_day) - int(duration_) + 1; lesson++ ) {
            if ( (resulting_available_start_time & (duration_mask << (day * lessons_in_day + lesson))) !=
                 duration_mask << (day * lessons_in_day + lesson) )
                resulting_available_start_time &= ~(static_cast<int64_t>(1) << (day * lessons_in_day + lesson) );
        }
        for ( int lesson = int(lessons_in_day) - int(duration_) + 1; lesson < lessons_in_day; lesson++)
            resulting_available_start_time &= ~(static_cast<int64_t>(1) << (day * lessons_in_day + lesson) );
    }

    return resulting_available_start_time;
}

int64_t CSubject::GetGroupAvailableTime() const {
    int64_t resulting_available_time(LLONG_MAX);

    for (const auto& group : groups_)
        resulting_available_time &= group->GetAvailableTime();

    return resulting_available_time;
}

int64_t CSubject::GetTeachersAvailableTime() const {
    int64_t resulting_available_time(LLONG_MAX);

    for (const auto& teacher : teachers_)
        resulting_available_time &= teacher->GetAvailableTime();

    return resulting_available_time;
}

size_t CSubject::GetTeachersAvailableTimeSize() const {
    int64_t resulting_available_time = GetTeachersAvailableTime() & GetGroupAvailableTime() & feasible_time_;

    size_t counter(0);
    for (int i = 0; i < INT64_SIZE; i++)
        if ( (resulting_available_time >> i) & 1 )
            counter++;

    return counter;
}

//______________________________________________________________________________________________________________________
// МЕТОДЫ  ДЛЯ  РАБОТЫ  С  ЗАНИМАЕМЫМ  И  ОСВОБОЖДАЕМЫМ  ВРЕМЕНЕМ
//______________________________________________________________________________________________________________________

void CSubject::ReserveTime(size_t start_time) {
    for (const auto& teacher : teachers_)
        teacher->ReserveTime(start_time, duration_);
    for (const auto& group : groups_)
        group->ReserveTime(start_time, duration_);
}

void CSubject::ReleaseTime(size_t start_time) {
    for (const auto& teacher : teachers_)
        teacher->ReleaseTime(start_time, duration_);
    for (const auto& group : groups_)
        group->ReleaseTime(start_time, duration_);
}

//______________________________________________________________________________________________________________________
//______________________________________________________________________________________________________________________
//
// CSubjectBuilder
//
//______________________________________________________________________________________________________________________
//______________________________________________________________________________________________________________________

//______________________________________________________________________________________________________________________
// СЕТТЕРЫ
//______________________________________________________________________________________________________________________

void CSubjectBuilder::SetSubjectName(std::string name) {
    name_ = name;
}

void CSubjectBuilder::SetSubjectId(size_t id) {
    id_ = id;
}

void CSubjectBuilder::SetSubjectDifficultyRating(size_t difficulty_rating) {
    difficulty_rating_ = difficulty_rating;
}

void CSubjectBuilder::SetSubjectDuration(size_t duration) {
    duration_ = duration;
}

void CSubjectBuilder::SetRequiredCabinetNumber(size_t required_cabinets_number) {
    required_cabinets_number_ = required_cabinets_number;
}

void CSubjectBuilder::SetFeasibleTime(int64_t feasible_time) {
    feasible_time_ = feasible_time;
}

void CSubjectBuilder::SetSubjectTeachers(std::set<CTeacher *const, Comparator<CTeacher>> teachers) {
    teachers_ = std::move(teachers);
}

void CSubjectBuilder::SetSubjectGroups(std::set<CGroup *const, Comparator<CGroup>> groups) {
    groups_ = std::move(groups);

    total_participants_ = 0;
    for (const auto& group : groups_ )
        total_participants_ += group->GetStudentsNumber();
}

void CSubjectBuilder::SetSubjectCabinets(std::set<CCabinet *const, Comparator<CCabinet>> cabinets) {
    cabinets_ = std::move(cabinets);
}

//______________________________________________________________________________________________________________________
// СОЗДАНИЕ  CSubject
//______________________________________________________________________________________________________________________

CSubject CSubjectBuilder::Build() const {
    return { name_,
             id_,
             difficulty_rating_,
             duration_,
             required_cabinets_number_,
             feasible_time_,
             teachers_,
             groups_,
             cabinets_,
             total_participants_ };
}

//______________________________________________________________________________________________________________________
//______________________________________________________________________________________________________________________
//
// CTimeTableGeneratorSupporter
//
//______________________________________________________________________________________________________________________
//______________________________________________________________________________________________________________________

//______________________________________________________________________________________________________________________
// ПРИВАТНЫЕ  МЕТОДЫ
//______________________________________________________________________________________________________________________

void CTimeTableGeneratorSupporter::moveTopToQueue() {
    priority_queue_.insert(stack_.back());
    stack_.pop_back();
}

void CTimeTableGeneratorSupporter::moveMinToStack() {
    CSubject *const current_subject = *priority_queue_.begin();
    stack_.push_back(current_subject);
    priority_queue_.erase(priority_queue_.begin());

    // Для переносимого предмета получаем возможные времена старта и запоминаем в стек времени случайную перестановку
    // этих времен.
    int64_t current_subject_availabel_time = current_subject->GetAvailableStartTime(days_in_week_, lessons_in_day_);
    times_stack_.push_back( RandomPermutation( current_subject_availabel_time ) );

    if ( current_subject_availabel_time == 0 ) {
        throw CBadSubjectPlacement("No available time for", current_subject);
    }

}

void CTimeTableGeneratorSupporter::backTrack( std::vector< std::pair<CSubject *, size_t> >& subjects_to_delete ) {
    // Главная функция backTrack -- откатиться к предыдущим предметам и поменять их время начала, чтобы попробовать
    // разместить те, которые не удалось разместить. После выхода из функции, на вершине стека предметов должен лежать
    // предмет, кторому нужно найти кабинет, а в стеке времени -- время его начала.

    // Пока нет времени начала для предмета
    do {
        // Если не пуст, выкидываем предыдущий вариант
        if ( !times_stack_.back().empty() )
            times_stack_.back().pop_back();

        // Если пуст, значит мы просмотрели все возможные времена для данного предмета и нам нужно перейти на
        // предыдущий, в порядке добавления в стек, предмет. Для этого перемещаем его в очередь и удаляем уже пустой
        // стек времен его начала. Если на этом этапе стек времен оказался пуст, значит мы проверили
        // все возможные варианты, и ни один нам не подошел -- составить расписание невозможно.
        // Также нужно удалить из расписания предмет (освободить кабинет, время учителей, групп и тд.),
        // который мы перенесли обратно в очередь.
        else {
            CleanStackTop();

            if ( times_stack_.empty() )
                throw CBadTimeTable("Can't create timetable");

            subjects_to_delete.emplace_back( std::make_pair(GetCurrentSubject(), GetCurrentSubjectStartTime()) );
            times_stack_.back().pop_back();
        }
    } while ( times_stack_.back().empty() );
    is_last_successful_ = true;
}

//______________________________________________________________________________________________________________________
// КОНСТРУКТОР
//______________________________________________________________________________________________________________________


CTimeTableGeneratorSupporter::CTimeTableGeneratorSupporter( std::map< std::string,CSubject > &subjects,
                                                            size_t days_in_week, size_t lessons_in_day )
        : days_in_week_(days_in_week),
          lessons_in_day_(lessons_in_day),
          is_last_successful_(true) {
    for (auto subject = subjects.begin(); subject != subjects.end(); subject++)
        priority_queue_.insert(&(*subject).second);
}

//______________________________________________________________________________________________________________________
// ФУНКЦИОНАЛ  ДЛЯ  ГЕНЕРАЦИИ
//______________________________________________________________________________________________________________________


void CTimeTableGeneratorSupporter::MakeIteration( std::vector< std::pair<CSubject *, size_t> >& subjects_to_delete ) {
    // Итерация генерации:
    // Если предыдущая итерация успешна и очередь предметов для заполнения не пуста, то мы готовы разместить в
    // расписании очередной предмет из очереди. Для этого переносим его в стек размещенных. Нас не интересует его
    // дальнейшая седьба, т.е. получится ли его разместить.
    // Если же предыдущая итерация неудачна, то есть либо для предмета не осталось доступных времен старта, либо ни для
    // одного времени старта нет подходящих (по времени и вместительности) кабинетов из числа возможных. Тогда выболняем
    // откат расписания. На момент отката на вершине стека должен лежать предмет, который не удалось разместить.
    // ПОСЛЕ  КАЖДОЙ  ИТЕРАЦИИ  НА  ВЕРШИНЕ  СТЕКА  ЛЕЖИТ  ПРЕДМЕТ,  ДЛЯ  КОТОРОГО  ИЩЕТСЯ  КАБИНЕТ.
    if ( is_last_successful_ )
        try {
            moveMinToStack();
        } catch (CBadSubjectPlacement& exc) {
            throw CBadSubjectPlacement("Can't place subject", exc.GetSubject());
        }
    else
        backTrack(subjects_to_delete);
}

void CTimeTableGeneratorSupporter::SetFailureFlag() {
    is_last_successful_ = false;
}

bool CTimeTableGeneratorSupporter::IsLastSuccessful() const {
    return is_last_successful_;
}

void CTimeTableGeneratorSupporter::CleanStackTop() {
    moveTopToQueue();
    times_stack_.pop_back();
}

void CTimeTableGeneratorSupporter::TimesStackPop() {
    times_stack_.back().pop_back();
}

bool CTimeTableGeneratorSupporter::QueueEmpty() const {
    return priority_queue_.empty();
}

bool CTimeTableGeneratorSupporter::StackEmpty() const {
    return stack_.empty();
}

bool CTimeTableGeneratorSupporter::CurrentSubjectTimesStackEmpty() const {
    return times_stack_.back().empty();
}

//______________________________________________________________________________________________________________________
// ГЕТТЕРЫ
//______________________________________________________________________________________________________________________

CSubject *const CTimeTableGeneratorSupporter::GetCurrentSubject() const {
    return stack_.back();
}

size_t CTimeTableGeneratorSupporter::GetCurrentSubjectStartTime() const {
    assert(!times_stack_.empty());
    assert(!times_stack_.back().empty());

    return times_stack_.back().back();
}
