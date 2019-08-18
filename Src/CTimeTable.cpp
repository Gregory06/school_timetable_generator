//
// Created by Gregory Postnikov on 2019-07-20.
//

#include "CTimeTable.h"
#include "CException.h"
#include <exception>
#include <fstream>
#include <sstream>
#include <cmath>

//______________________________________________________________________________________________________________________
//______________________________________________________________________________________________________________________
//
// CTimeTable
//
//______________________________________________________________________________________________________________________
//______________________________________________________________________________________________________________________

//______________________________________________________________________________________________________________________
// ПРИВАТНЫЕ  МЕТОДЫ
//______________________________________________________________________________________________________________________

CTimeTable::CTimeTable(std::map<std::string, CTeacher>& teachers, std::map<std::string, CCabinet>& cabinets,
                       std::map<std::string, CGroup>& groups, std::map<std::string, CSubject>& subjects,
                       size_t days_in_week, size_t lessons_in_day):
                       teachers_(teachers),
                       cabinets_(cabinets),
                       groups_(groups),
                       event_linker_(subjects, groups),
                       days_in_week_(days_in_week),
                       lessons_in_day_(lessons_in_day) {

    // Для копирования в teachers_, cabinets_ и groups_ можно воспользоваться конструктором по умолчанию, так как
    // достаточно поверхностного копирования. Для копирования в subjects_ нужно "переподвязать"
    // соответсвующие указатели.

    for (auto& pair : subjects) {
        CSubject subject = pair.second;
        CSubjectBuilder subject_builder;

        subject_builder.SetSubjectName(subject.GetName());
        subject_builder.SetSubjectId(subject.GetId());
        subject_builder.SetSubjectDuration(subject.GetDuration());
        subject_builder.SetRequiredCabinetNumber(subject.GetRequiredCabinetsNumber());
        subject_builder.SetFeasibleTime(subject.GetFeasibleTime());

        std::set< CGroup *const, Comparator<CGroup> > subject_groups;
        for (auto& subject_group : subject.GetGroups())
            subject_groups.insert( &groups_.at(subject_group->GetName()) );

        std::set< CCabinet *const, Comparator<CCabinet> > subject_cabinets;
        for (auto& subject_cabinet : subject.GetCabinets())
            subject_cabinets.insert( &cabinets_.at(subject_cabinet->GetName()) );

        std::set< CTeacher *const, Comparator<CTeacher> > subject_teachers;
        for (auto& subject_teacher : subject.GetTeachers())
            subject_teachers.insert( &teachers_.at(subject_teacher->GetName()) );

        subject_builder.SetSubjectGroups(subject_groups);
        subject_builder.SetSubjectTeachers(subject_teachers);
        subject_builder.SetSubjectCabinets(subject_cabinets);

        subjects_.insert( std::make_pair(pair.first, subject_builder.Build()) );
    }

    for (const auto& i : groups_)
        time_table_.insert( { i.first, std::vector<CEvent>(days_in_week * lessons_in_day) } );
}

void CTimeTable::insertEvent( CSubject *const subject,
                              const std::set<CCabinet *const, Comparator<CCabinet>>& cabinets,
                              size_t start_time ) {
    assert(subject);

    std::string current_group_name;

    for ( const auto& group : subject->GetGroups() ) {
        current_group_name = group->GetName();

        // Заносим событие во все группы на всю длину предмета
        for ( int i = 0; i < subject->GetDuration(); i++ ) {
            time_table_.at(current_group_name)[start_time + i].SetSubject(subject);
            time_table_.at(current_group_name)[start_time + i].SetCabinets(cabinets);
            time_table_.at(current_group_name)[start_time + i].SetStartTime(start_time);
        }

        // Важно, что в event_linker_ событие заносится только после добавления в само расписание, т.е. time_table_,
        // а удаляется в обратном порядке.
        event_linker_.InsertEvent( current_group_name, time_table_.at(current_group_name)[start_time] );
    }

    // Блокируем время у всех участников события
    subject->ReserveTime(start_time);
    for (const auto& cabinet : cabinets)
        cabinet->ReserveTime(start_time, subject->GetDuration());
}

void CTimeTable::deleteEvent( CSubject *const subject,
                              size_t start_time ) {
    assert(subject);

    subject->ReleaseTime(start_time);

    std::string current_group_name;
    for ( const auto& group : subject->GetGroups() ) {
        current_group_name = group->GetName();

        // Важно, что в event_linker_ событие заносится только после добавления в само расписание, т.е. time_table_,
        // а удаляется в обратном порядке.
        event_linker_.DeleteEvent( current_group_name, time_table_.at(current_group_name)[start_time] );

        for ( int i = 0; i < subject->GetDuration(); i++ )
            time_table_.at(current_group_name)[start_time + i].SetSubject(nullptr);
    }

    // Освобождаем врямя всех участников
    const auto& cabinets = GetEvent( current_group_name, start_time ).GetCabinets();
    for (const auto& cabinet : cabinets)
        cabinet->ReleaseTime(start_time, subject->GetDuration());
}

auto CTimeTable::findFeasibleCabinet( CTimeTableGeneratorSupporter& supporter ) const {
    CSubject *const subject = supporter.GetCurrentSubject();

    // Ищем во всех доступных для данного события временах, т.е. стеке времен
    while ( !supporter.CurrentSubjectTimesStackEmpty() ) {
        std::set<CCabinet *const, Comparator<CCabinet>> feasible_cabinets;
        size_t start_time = supporter.GetCurrentSubjectStartTime();

        for ( const auto& cabinet : subject->GetCabinets() ) {

            // Проверка на вместимость кабинетом всех учащихся
            if (cabinet->GetCapacity() < subject->GetParticipantsNumber())
                continue;
            // Проверка на незанятость кабинета на всю длину предмета от рассматриваемого начального времени
            if (!cabinet->IsFeasible(start_time, subject->GetDuration()))
                continue;

            // Как только нашли нужное для проведения предмета количество кобинетов, возвращаем
            feasible_cabinets.insert(cabinet);
            if ( feasible_cabinets.size() == subject->GetRequiredCabinetsNumber() )
                return std::move(feasible_cabinets);
        }

        supporter.TimesStackPop();
    }

    // Окажемся здесь, только если не нашли нужное количество кабинетов ни в какое время.
    throw CBadCabinetsFind("Can't find cabinet for", subject);
}

auto CTimeTable::findFeasibleCabinet( CSubject *subject, size_t start_time ) const {
    std::set<CCabinet *const, Comparator<CCabinet>> feasible_cabinets;

    for ( const auto& cabinet : subject->GetCabinets() ) {

        // Проверка на вместимость кабинетом всех учащихся
        if (cabinet->GetCapacity() < subject->GetParticipantsNumber())
            continue;

        // Проверка на незанятость кабинета на всю длину предмета от рассматриваемого начального времени
        if (!cabinet->IsFeasible(start_time, subject->GetDuration()))
            continue;

        // Как только нашли нужное для проведения предмета количество кобинетов, возвращаем
        feasible_cabinets.insert(cabinet);
        if ( feasible_cabinets.size() == subject->GetRequiredCabinetsNumber() ) {
            return std::move(feasible_cabinets);
        }
    }

    // Не выкидываем исключение, так как при работе этой версии фукции предполагается проверка на вызывающей стороне
    return std::set<CCabinet *const, Comparator<CCabinet>> {};
}

bool CTimeTable::swappable(const CEvent &from, const CEvent &to) {
    if ( !from.IsActive() || !to.IsActive() )
        return false;

    // один и то же предмет нет смылса менять местами с собой
    if ( from.GetSubject() == to.GetSubject() )
        return false;

    // Проверка на "перекрытие", т.е. что события достаточно отстают друг от друга, чтобы после
    // перестановки раннее событие не перекрыло позднее своим концом.
    if ( abs(static_cast<int>(to.GetStartTime()) - static_cast<int>(from.GetStartTime()) ) <
         std::max(to.GetSubject()->GetDuration(), from.GetSubject()->GetDuration()) )
        return false;

    // Создаем временные версии событий, чтобы восстановить удаленные для проверки оригиналы
    CEvent from_temp(from), to_temp(to);

    // Удаляем, чтобы посмотеть, встанет ли первое событие на место второго и наоборот
    deleteEvent(from.GetSubject(), from.GetStartTime());
    deleteEvent(to.GetSubject(), to.GetStartTime());

    bool output( movable(from_temp, to_temp.GetStartTime()) && movable(to_temp, from_temp.GetStartTime()) );

    // После проверки возвращаем все обратно с помощью временных объектов
    insertEvent(from_temp.GetSubject(), from_temp.GetCabinets(), from_temp.GetStartTime());
    insertEvent(to_temp.GetSubject(), to_temp.GetCabinets(), to_temp.GetStartTime());

    return output;
}

void CTimeTable::swap(CEvent &from, CEvent &to) {
    CEvent from_temp(from), to_temp(to);

    // Удаляем старые события
    deleteEvent(from.GetSubject(), from.GetStartTime());
    deleteEvent(to.GetSubject(), to.GetStartTime());

    // Заносим события на место противоположного с помощью временных объектов
    insertEvent(from_temp.GetSubject(), findFeasibleCabinet(from_temp.GetSubject(), to_temp.GetStartTime()), to_temp.GetStartTime());
    insertEvent(to_temp.GetSubject(), findFeasibleCabinet(to_temp.GetSubject(), from_temp.GetStartTime()), from_temp.GetStartTime());

}

bool CTimeTable::movable(const CEvent &from, size_t new_start_time) {
    if ( !from.IsActive() )
        return false;

    // Проверяем, входит ли new_start_time в множество возможных в данный момент времен начала для данного предмета
    int64_t available_start_time = from.GetSubject()->GetAvailableStartTime(days_in_week_, lessons_in_day_);

    if ( (available_start_time & (static_cast<int64_t>(1) << new_start_time)) !=
         (static_cast<int64_t>(1) << new_start_time) )
        return false;

    // Проверяем, найшелся ли кабинет для данного предмета на новое время
    if ( findFeasibleCabinet(from.GetSubject(), new_start_time).size() < from.GetSubject()->GetRequiredCabinetsNumber() )
        return false;

    return true;
}

bool CTimeTable::move(const CEvent &from, size_t new_start_time) {
    insertEvent(from.GetSubject(), findFeasibleCabinet(from.GetSubject(), new_start_time), new_start_time);
    deleteEvent(from.GetSubject(), from.GetStartTime());
}

//______________________________________________________________________________________________________________________
// КОНСТРУКТОРЫ
//______________________________________________________________________________________________________________________

// TODO
// Очень неоптимально : скорее всего куча узких мест, которые тормозят программу
CTimeTable::CTimeTable(const CTimeTable &timetable)
        : teachers_(timetable.teachers_),
          cabinets_(timetable.cabinets_),
          groups_(timetable.groups_),
          event_linker_(timetable.subjects_, timetable.groups_),
          days_in_week_(timetable.days_in_week_),
          lessons_in_day_(timetable.lessons_in_day_) {

    for (auto& pair : timetable.subjects_) {
        CSubject subject = pair.second;
        CSubjectBuilder subject_builder;

        subject_builder.SetSubjectName(subject.GetName());
        subject_builder.SetSubjectId(subject.GetId());
        subject_builder.SetSubjectDuration(subject.GetDuration());
        subject_builder.SetRequiredCabinetNumber(subject.GetRequiredCabinetsNumber());
        subject_builder.SetFeasibleTime(subject.GetFeasibleTime());

        std::set< CGroup *const, Comparator<CGroup> > subject_groups;
        for (auto& subject_group : subject.GetGroups())
            subject_groups.insert( &groups_.at(subject_group->GetName()) );

        std::set< CCabinet *const, Comparator<CCabinet> > subject_cabinets;
        for (auto& subject_cabinet : subject.GetCabinets())
            subject_cabinets.insert( &cabinets_.at(subject_cabinet->GetName()) );

        std::set< CTeacher *const, Comparator<CTeacher> > subject_teachers;
        for (auto& subject_teacher : subject.GetTeachers())
            subject_teachers.insert( &teachers_.at(subject_teacher->GetName()) );

        subject_builder.SetSubjectGroups(subject_groups);
        subject_builder.SetSubjectTeachers(subject_teachers);
        subject_builder.SetSubjectCabinets(subject_cabinets);


        subjects_.insert( std::make_pair(pair.first, subject_builder.Build()) );
    }

    for (const auto& i : groups_)
        time_table_.insert( { i.first, std::vector<CEvent>(days_in_week_ * lessons_in_day_) } );

    for (const auto& [group_name, events] : timetable.time_table_)
        for (int time = 0; time < events.size(); time++) {
            if ( !events[time].IsActive() ) {
                continue;
            }
            time_table_.at(group_name)[time].SetSubject(&subjects_.at(events[time].GetSubject()->GetName()));

            std::set<CCabinet *const, Comparator<CCabinet>> cabinets {};
            for (const auto& cabinet : events[time].GetCabinets()) {
                cabinets.insert(&cabinets_.at(cabinet->GetName()));
            }
            time_table_.at(group_name)[time].SetCabinets(cabinets);
            time_table_.at(group_name)[time].SetStartTime(events[time].GetStartTime());
        }

    for (const auto& [group_name, events] : time_table_)
        for (int time = 0; time < events.size();) {
            if ( !events[time].IsActive() ) {
                time++;
                continue;
            }
            event_linker_.InsertEvent(group_name, time_table_.at(group_name)[time]);
            time += time_table_.at(group_name)[time].GetSubject()->GetDuration();
        }
}

// TODO
// Очень неоптимально : скорее всего куча узких мест, которые тормозят программу
CTimeTable& CTimeTable::operator=(const CTimeTable &timetable) {

    teachers_.clear();
    cabinets_.clear();
    groups_.clear();
    subjects_.clear();
    time_table_.clear();

    for (const auto& teacher : timetable.teachers_)
        teachers_.insert( teacher );
    for (const auto& cabinet : timetable.cabinets_)
        cabinets_.insert( cabinet );
    for (const auto& group : timetable.groups_)
        groups_.insert( group );

    days_in_week_ = timetable.days_in_week_;
    lessons_in_day_ = timetable.lessons_in_day_;

    for (auto& pair : timetable.subjects_) {
        CSubject subject = pair.second;
        CSubjectBuilder subject_builder;

        subject_builder.SetSubjectName(subject.GetName());
        subject_builder.SetSubjectId(subject.GetId());
        subject_builder.SetSubjectDuration(subject.GetDuration());
        subject_builder.SetRequiredCabinetNumber(subject.GetRequiredCabinetsNumber());
        subject_builder.SetFeasibleTime(subject.GetFeasibleTime());

        std::set< CGroup *const, Comparator<CGroup> > subject_groups;
        for (auto& subject_group : subject.GetGroups())
            subject_groups.insert( &groups_.at(subject_group->GetName()) );

        std::set< CCabinet *const, Comparator<CCabinet> > subject_cabinets;
        for (auto& subject_cabinet : subject.GetCabinets())
            subject_cabinets.insert( &cabinets_.at(subject_cabinet->GetName()) );

        std::set< CTeacher *const, Comparator<CTeacher> > subject_teachers;
        for (auto& subject_teacher : subject.GetTeachers())
            subject_teachers.insert( &teachers_.at(subject_teacher->GetName()) );

        subject_builder.SetSubjectGroups(subject_groups);
        subject_builder.SetSubjectTeachers(subject_teachers);
        subject_builder.SetSubjectCabinets(subject_cabinets);

        subjects_.insert( std::make_pair(pair.first, subject_builder.Build()) );
    }

    event_linker_ = CEventLinker(subjects_, groups_);

    for (const auto& i : groups_)
        time_table_.insert( { i.first, std::vector<CEvent>(days_in_week_ * lessons_in_day_) } );

    for (const auto& [group_name, events] : timetable.time_table_)
        for (int time = 0; time < events.size(); time++) {
            if ( !events[time].IsActive() ) {
                continue;
            }
            time_table_.at(group_name)[time].SetSubject(&subjects_.at(events[time].GetSubject()->GetName()));

            std::set<CCabinet *const, Comparator<CCabinet>> cabinets {};
            for (const auto& cabinet : events[time].GetCabinets()) {
                cabinets.insert(&cabinets_.at(cabinet->GetName()));
            }
            time_table_.at(group_name)[time].SetCabinets(cabinets);
            time_table_.at(group_name)[time].SetStartTime(events[time].GetStartTime());
        }

    for (const auto& [group_name, events] : time_table_)
        for (int time = 0; time < events.size();) {
            if ( !events[time].IsActive() ) {
                time++;
                continue;
            }
            event_linker_.InsertEvent(group_name, time_table_.at(group_name)[time]);
            time += time_table_.at(group_name)[time].GetSubject()->GetDuration();
        }

    return *this;
}

//______________________________________________________________________________________________________________________
// ИНТЕРФЕЙС  ДЛЯ  ОПТИМИЗАТОРОВ
//______________________________________________________________________________________________________________________

const int MAX_ATTEMPTS_COUNT (10);
const int MAX_ITERATION_COUNT (10000);

// Точка входа в генерацию корректного случайного расписания
void CTimeTable::GenerateTimeTable() {

    // Возможно такое, что попытка создать расписание уйдет в экспоненциальную сложность, тогда следует прервать
    // генерацию и начать сначала. Для этого служит счетчик iteration_counter. Его предельное значение должно зависеть
    // от длины расписания => TODO
    // Проверка невозможности создать расписание так же NPC, следовательно, если не получается создать расписание
    // некоторое количество раз подряд, выкидываем исключение. Предельное значение количества попыток
    // тоже должно зависеть от длины => TODO
    int attempts_counter(0);
    while (true) {
        if (attempts_counter++ > MAX_ATTEMPTS_COUNT)
            throw CBadTimeTable("Can't create timetable");
        int iteration_counter(0);
        // Флаг удачности попытки
        bool bad_try(false);

        // Хранилище предметов -- очередь с приоритетом по занятости
        // преподавателей + стек добавлений в расписание. В каждый момент
        // времени их объединение дает множество всех предметов в учебном плане.
        CTimeTableGeneratorSupporter generator_supporter(subjects_,
                                                         days_in_week_, lessons_in_day_);

        // Пока очередь предметов для размещения в расписании не пуста.
        // При этом, даже если очередь уже пуста, необходимо, чтобы последнее размещение
        // прошло удачно, так как, в противном случае, расписание еще не корректно.
        while (!generator_supporter.QueueEmpty() || !generator_supporter.IsLastSuccessful()) {

            if (iteration_counter++ > MAX_ITERATION_COUNT) {
                RecoverTimeTable();
                bad_try = true;
                break;
            }

            std::set<CCabinet *const, Comparator<CCabinet>> feasible_cabinets;

            try {

                // В subjects_to_delete после выполнения MakeIteration будут находиться
                // пары -- предмет, который нужно удалить из расписания x время начала, события, представляющего
                // этот предмет. Если ничего удалять не нужно, то и subjects_to_delete будет пуст.
                std::vector< std::pair<CSubject *, size_t> > subjects_to_delete;
                // MakeIteration оставит на вершине стека предметов вспомогательного класса generator_supporter
                // предмет, который нужно разместить в расписании на время, находящееся на вершине стека времени, или
                // выкидывает исключение, если стек времени оказался пуст. Если стек предметов оказался пуст, значит мы
                // проверили все возможные варианты расписаний и ни одно не оказалось корректным.
                generator_supporter.MakeIteration(subjects_to_delete);
                // Удаление событий после бэктрэка
                for (const auto &subject_to_delete : subjects_to_delete)
                    deleteEvent(subject_to_delete.first, subject_to_delete.second);

                try {
                    // Если исключение на предыдущем шаге выброшено не было, значит на вершине стека предметов
                    // лежит предмет, ожидающий размещение в расписание.
                    feasible_cabinets = findFeasibleCabinet(generator_supporter);
                } catch (CBadCabinetsFind &exc) {
                    throw CBadSubjectPlacement("Can't place subject", exc.GetSubject());
                }

            } catch (CBadSubjectPlacement &exc) {
                // Если на одном из этапов было выброшено исключение, значит размещение очередного предмета завершилось
                // неудачей. Для того, чтобы сделать бэетрэк на следующей итерации, ставим флаг.
                generator_supporter.SetFailureFlag();
                continue;
            }

            // Если дошли до этого места, значит готовы к размещению в рассписание нового события
            insertEvent(generator_supporter.GetCurrentSubject(),
                        feasible_cabinets,
                        generator_supporter.GetCurrentSubjectStartTime());
        }

        // Если попытка не неудачная, выходим
        if (!bad_try)
            break;
    }
}

void CTimeTable::RecoverTimeTable() {
    for (auto& teacher : teachers_)
        teacher.second.RecoverTeacher();

    for(auto& group : groups_)
        group.second.RecoverGroup();

    for(auto& cabinet : cabinets_)
        cabinet.second.RecoverCabinet();

    for(auto& group_schedule : time_table_)
        for (auto& event : group_schedule.second)
            event.FreeEvent();

    event_linker_.FreeEvents();
}

void CTimeTable::RandomSwap() {
    // Создаем векторы времен, заполняем их случайной перестановкой [0..days_in_week_*lessons_in_day_-1]
    // Создаем вектор случайных перестановок имен групп
    std::vector<size_t> times_from(days_in_week_*lessons_in_day_), times_to(days_in_week_*lessons_in_day_);
    std::vector<std::string> groups;

    std::iota(times_from.begin(), times_from.end(), 0);
    std::iota(times_to.begin(), times_to.end(), 0);
    RandomPermutation(times_from);
    RandomPermutation(times_to);

    for (const auto& group : groups_)
        groups.push_back(group.first);
    RandomPermutation(groups);

    // Проходим по всем вариантам пар событий, проверяем на "переставляемость".
    // Если находим -- меняем местами.
    for (auto& group : groups)
        for (auto time_from : times_from)
            for (auto time_to : times_to)
                if ( swappable( time_table_.at(group)[time_from],
                                time_table_.at(group)[time_to] ) ) {
                    swap( time_table_.at(group)[time_from],
                          time_table_.at(group)[time_to] );
                    return;
                }
}

void CTimeTable::RandomMove() {
    // Создаем векторы времен, заполняем их случайной перестановкой [0..days_in_week_*lessons_in_day_-1]
    // Создаем вектор случайных перестановок имен групп
    std::vector<size_t> times_from(days_in_week_*lessons_in_day_), times_to(days_in_week_*lessons_in_day_);
    std::vector<std::string> groups;

    std::iota(times_from.begin(), times_from.end(), 0);
    std::iota(times_to.begin(), times_to.end(), 0);
    RandomPermutation(times_from);
    RandomPermutation(times_to);

    for (const auto& group : groups_)
        groups.push_back(group.first);
    RandomPermutation(groups);

    // Проходим по всем вариантам пар (событие, время), проверяем на "переносимость".
    // Если находим -- переносим.
    for (auto& group : groups)
        for (auto time_from : times_from)
            for (auto time_to : times_to)
                if ( movable( time_table_.at(group)[time_from], time_to ) ) {
                    move( time_table_.at(group)[time_from], time_to );
                    return;
                }
}

//______________________________________________________________________________________________________________________
// ГЕТТЕРЫ
//______________________________________________________________________________________________________________________

const CSubject& CTimeTable::GetSubject(std::string subject_name) const {
    return subjects_.at(subject_name);
}

const CEvent& CTimeTable::GetEvent(std::string group_name, size_t start_time) const {
    return time_table_.at(group_name)[start_time];
}

//______________________________________________________________________________________________________________________
// МЕТОДЫ  ДЛЯ  ВЫВОДА
//______________________________________________________________________________________________________________________

void CTimeTable::GroupsScheduleTex(std::string filename) const {
    std::ofstream file(filename);

    if ( !file.is_open() ) {
        std::cout << "ERREE" <<std::endl;
        throw std::exception();
    }

    file << "\\documentclass[a4paper,12pt]{article} \n"
            "\\usepackage[english,russian]{babel}\n"
            "\\usepackage[left=0.5cm,right=0.5cm,\n"
            "    top=0.5cm,bottom=0.5cm,bindingoffset=0cm]{geometry}\n"
            "\\begin{document}\n"
            "\\begin{center}\n"
            "\\tiny\n";

    for (auto & group : time_table_) {
        file << "\\begin{tabular}{ | c |  } \\hline \n";
        file << group.first << " \\\\ \\hline \n";

        auto& lessons = group.second;
        for (int lesson = 0; lesson < lessons_in_day_; lesson++) {
            file <<  "\\begin{tabular}{ *{" << days_in_week_<<"}{| p {90pt} |} } \n";
            for (int day = 0; day < days_in_week_; day++) {
                file <<  "\\begin{tabular}{  c   } \n";
                if ( !lessons[day*lessons_in_day_+lesson].IsActive() ) {
                    file << "\\\\ \n";
                    file << "\\\\ \n";
                    file << "\\\\ \n";
                } else {
                    file  << lessons[day*lessons_in_day_+lesson].GetName() << "\\\\ \n";
                    for (auto& teacher : lessons[day*lessons_in_day_+lesson].GetTeachers())
                        file  << teacher->GetName() << ", ";
                    file << "\\\\ \n";
                    for (auto& cabinet : lessons[day*lessons_in_day_+lesson].GetCabinets())
                        file  << cabinet->GetName() << ", ";
                    file << "\\\\ \n";
                }

                file << "\\end{tabular}\n";
                if (day != days_in_week_-1)
                    file << " &\n";
            }

            file << "\\end{tabular} \\\\ \\hline \n";
        }

        file << "\\end{tabular} \n";
        file << "\\\\ \n";
    }

    file << "\\end{center}\n";
    file << "\\end{document}\n";

    file.close();
}

void CTimeTable::TeachersScheduleTex(std::string filename) const {
    std::ofstream file(filename);

    if ( !file.is_open() ) {
        std::cout << "ERREE" <<std::endl;
        throw std::exception();
    }

    file << "\\documentclass[a4paper,12pt]{article} \n"
            "\\usepackage[english,russian]{babel}\n"
            "\\usepackage[left=0.5cm,right=0.5cm,\n"
            "    top=0.5cm,bottom=0.5cm,bindingoffset=0cm]{geometry}\n"
            "\\begin{document}\n"
            "\\begin{center}\n"
            "\\tiny\n";

    for (auto [teacher_name, teacher] : teachers_) {

        file << "\\begin{tabular}{ | c |  } \\hline \n";
        file << teacher_name << " \\\\ \\hline \n";

        for (int lesson = 0; lesson < lessons_in_day_; lesson++) {
            file << "\\begin{tabular}{ *{" << days_in_week_ << "}{| p {90pt} |} } \n";
            for (int day = 0; day < days_in_week_; day++) {
                file << "\\begin{tabular}{  c   } \n";
                for (auto & [group_name, lessons] : time_table_) {
                    if (!lessons[day * lessons_in_day_ + lesson].IsActive()) {
                        file << "\\\\ \n";
                    } else if ( lessons[day * lessons_in_day_ + lesson].GetTeachers().end() !=
                                lessons[day * lessons_in_day_ + lesson].GetTeachers().find(&teacher) ) {
                        file << lessons[day * lessons_in_day_ + lesson].GetName() << "\n";
                    }
                }

                file << "\\end{tabular}\n";
                if (day != days_in_week_ - 1)
                    file << " &\n";
            }

            file << "\\end{tabular} \\\\ \\hline \n";
        }

        file << "\\end{tabular} \n";
        file << "\\\\ \n";
    }

    file << "\\end{center}\n";
    file << "\\end{document}\n";

    file.close();
}

//______________________________________________________________________________________________________________________
// ЧТЕНИЕ  ИЗ  ФАЙЛОВ
//______________________________________________________________________________________________________________________

// Вспомогательная функция добавляющая универсальности при чтении различных типов данных из файла
// На вход : имя файла для чтения, указатель на CTimeTableBuilder в который происходит запись,
// указатель на функцию парсинга строки, принимающую на вход rvalue ссылку на std::stringstream
// и тот же указатель на CTimeTableBuilder.
// Указатель на CTimeTableBuilder нужен для того, чтобы связывать предметы с группами и преподавателями при
// чтении файла предметов, так как предполагается, что в качестве указателя на функцию будет передаваться
// лямбда-функция. Если не прокинуть в лямбда-функцию укзатель на CTimeTableBuilder, то в ней не будет доступа к
// полям уже заполненным полям CTimeTableBuilder, т.е. teachers_ и тд. Но по действующим стандартам лямбда-функция,
// в которую прокидываются значения не может выступать в роли указателя на функцию. Поэтому передаем this как обычный
// параметр.
template <typename T>
std::map<std::string, T> ReadMapFromFile( const std::string& filename,
                                          CTimeTableBuilder* ptr,
                                          T (*parse_func) (std::stringstream&& , CTimeTableBuilder* ) ) {
    std::ifstream file(filename);

    if ( !file.is_open() )
        throw std::exception();

    std::string line;
    std::map<std::string, T> output;

    while(std::getline(file, line)) {
        T obj = parse_func( std::stringstream(line), ptr );
        output.insert( { obj.GetName(), obj } );
    }

    file.close();

    return std::move(output);
}

//______________________________________________________________________________________________________________________
//______________________________________________________________________________________________________________________
//
// CTimeTableBuilder
//
//______________________________________________________________________________________________________________________
//______________________________________________________________________________________________________________________

//______________________________________________________________________________________________________________________
// СЕТТЕРЫ,  ВЫЗЫВАЮЩИЕ  ReadMapFromFile И РЕАЛИЗУЮЩИЕ  НУЖНЫЕ  ДЛЯ  ПАРСИНГА  ФУНКЦИИ
//______________________________________________________________________________________________________________________

void CTimeTableBuilder::SetTimeTableTeachers(std::string teachers_filename) {

    teachers_ = ReadMapFromFile<CTeacher>( teachers_filename, this,
                                           [] (std::stringstream&& line, CTimeTableBuilder* ptr) {
        std::string name;
        int64_t avaliable_time;
        std::vector<size_t> time_rating;

        // TEMPORARY TODO
        std::string time;

        line >> name;
        line >> time;

        time_rating.resize(time.length());
        for (int i = time_rating.size()-1; i > -1; i--) {
            line >> time_rating[i];
        }

        avaliable_time = Str2Int64(time);

        return CTeacher{ name, avaliable_time, time_rating };
    } );

}

void CTimeTableBuilder::SetTimeTableGroups(std::string groups_filename) {

    groups_ = ReadMapFromFile<CGroup>( groups_filename, this,
                                       [] (std::stringstream&& line, CTimeTableBuilder* ptr) {

        std::string name;
        size_t students_number;
        int64_t avaliable_time;

        // TEMPORARY TODO
        std::string time;

        line >> name;
        line >> students_number;
        line >> time;

        avaliable_time = Str2Int64(time);

        return CGroup{ name, students_number, avaliable_time };
    } );

}

void CTimeTableBuilder::SetTimeTableCabinets(std::string cabinets_filename) {

    cabinets_ = ReadMapFromFile<CCabinet>( cabinets_filename, this,
                                           [] (std::stringstream&& line, CTimeTableBuilder* ptr) {
        std::string name;
        size_t capacity;
        int64_t avaliable_time;

        // TEMPORARY TODO
        std::string time;

        line >> name;
        line >> capacity;
        line >> time;

        avaliable_time = Str2Int64(time);

        return CCabinet{ name, capacity, avaliable_time };
    } );

}

void CTimeTableBuilder::SetTimeTableSubjects(std::string subjects_filename) {

    subjects_ = ReadMapFromFile<CSubject>( subjects_filename, this,
                                           [] (std::stringstream&& line, CTimeTableBuilder* ptr) {

        std::string name;
        size_t id;
        size_t difficulty_rating;
        size_t duration;
        size_t required_cabinets_number;
        std::set<CTeacher *const, Comparator<CTeacher>> teachers;
        std::set<CGroup *const, Comparator<CGroup>> groups;
        std::set<CCabinet *const, Comparator<CCabinet>> cabinets;

        // TEMPORARY TODO
        std::string time;

        line >> name;
        line >> id;
        line >> difficulty_rating;
        line >> duration;
        line >> required_cabinets_number;
        line >> time;

        int teachers_number;
        line >> teachers_number;

        std::string teacher_name;
        for (int i = 0; i < teachers_number; i++) {
            line >> teacher_name;
            if (ptr->teachers_.find(teacher_name) == ptr->teachers_.end())
                throw CException("Teacher is not found " + teacher_name + " in subject " + name);
            teachers.insert(& (ptr->teachers_.at(teacher_name)) );
        }

        int groups_number;
        line >> groups_number;
        std::string group_name;
        for (int i = 0; i < groups_number; i++) {
            line >> group_name;
            if (ptr->groups_.find(group_name) == ptr->groups_.end())
                throw CException("Group is not found " + group_name + " in subject " + name);
            groups.insert(& (ptr->groups_.at(group_name)) );
        }

        int cabinets_number;
        line >> cabinets_number;
        std::string cabinet_name;
        for (int i = 0; i < cabinets_number; i++) {
            line >> cabinet_name;
            if (ptr->cabinets_.find(cabinet_name) == ptr->cabinets_.end())
                throw CException("Cabinet is not found " + name);
            cabinets.insert(& (ptr->cabinets_.at(cabinet_name)) );
        }

        CSubjectBuilder subject_builder;

        subject_builder.SetSubjectName(name);
        subject_builder.SetSubjectId(id);
        subject_builder.SetSubjectDifficultyRating(difficulty_rating);
        subject_builder.SetSubjectDuration(duration);
        subject_builder.SetRequiredCabinetNumber(required_cabinets_number);
        subject_builder.SetFeasibleTime(Str2Int64(time));
        subject_builder.SetSubjectTeachers(teachers);
        subject_builder.SetSubjectGroups(groups);
        subject_builder.SetSubjectCabinets(cabinets);

        return subject_builder.Build();
    } );

}

void CTimeTableBuilder::SetTimeTableSize(size_t days_in_week, size_t lessons_in_day) {
    days_in_week_ = days_in_week;
    lessons_in_day_ = lessons_in_day;
}

//______________________________________________________________________________________________________________________
// СОЗДАНИЕ  CTimeTable
//______________________________________________________________________________________________________________________

CTimeTable CTimeTableBuilder::Build() {

    return { teachers_,
             cabinets_,
             groups_,
             subjects_,
             days_in_week_, lessons_in_day_ };

}