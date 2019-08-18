//
// Created by Gregory Postnikov on 2019-07-20.
//

#ifndef TIMER_CTIMETABLE_H
#define TIMER_CTIMETABLE_H

#include "CTeacher.h"
#include "CCabinet.h"
#include "CGroup.h"
#include "CEvent.h"

class CTimeTableBuilder;
class CTimeTableGeneratorSupporter;

// Основной класс, содержащий само расписание,
// данные о группах, кабинетах, учителях, предметах, времени на неделе,
// обладающий интерфейсы для работы с классами оптимизации :
// GenerateTimeTable, RandomSwap, RandomMove.
//______________________________________________________________________________________________________________________
class CTimeTable {
private:

    // Данные для построения расписсания.
    // Каждая копия CTimeTable хранит свои данные,
    // так как при построении происходит оперирование с указателями на данные.
    std::map< std::string, CTeacher > teachers_;
    std::map< std::string, CCabinet > cabinets_;
    std::map< std::string, CGroup > groups_;

    std::map< std::string, CSubject > subjects_;

    size_t days_in_week_, lessons_in_day_;

    // Само расписание: Группа x N -> Событие
    std::map< std::string, std::vector<CEvent> > time_table_;

    // Специальный объект для связи событий, представляющих копии одних и тех же предметов.
    CEventLinker event_linker_;

    // Конструктор недоступен. Для создания используется
    // вспомогательный класс CTimeTableBuilder ( метод Build )
    CTimeTable( std::map< std::string, CTeacher >& teachers,
                std::map< std::string, CCabinet >& cabinets,
                std::map< std::string, CGroup >& groups,
                std::map< std::string, CSubject >& subjects,
                size_t days_in_week, size_t lessons_in_day );

    // Добавить событие в расписание ( предмет в указанное время в указанных(ом) кабинетах(те) ).
    // При добавлении, время, занимаемое событием, блокируется у всех участников, т.е. учителей,
    // групп и классов.
    void insertEvent( CSubject *const subject,
                      const std::set<CCabinet *const, Comparator<CCabinet>>& cabinets,
                      size_t start_time );
    // Удалить событие в расписание ( предмет в указанное время ).
    // При удалении, время, занимаемое событием, освобождается у всех участников, т.е. учителей,
    // групп и классов.
    void deleteEvent( CSubject *const subject,
                      size_t start_time );

    // Найти подходящий кабинет по состоянию помощника (CTimeTableGeneratorSupporter).
    // В качестве предмета берется предмет на вершине стека предметов,
    // в качестве времени -- время на вершине стека времени.
    auto findFeasibleCabinet( CTimeTableGeneratorSupporter& supporter ) const;
    // Найти кабинет непосредственно для предмета subject с началом в start_time.
    auto findFeasibleCabinet( CSubject* subject, size_t start_time ) const;

    // true, если можно поменять местами события без нарушения коректности.
    // false, иначе.
    // При любом исходе после завершения функции объект класса находится в том же
    // состоянии, что и до запуска. В методе происходит модификация объекта во время работы, поэтому нельзя
    // использовать для константных объектов.
    bool swappable(const CEvent& from, const CEvent& to);
    // Поменять местами события. Проверка на коректность не производится.
    void swap(CEvent& from, CEvent& to);

    // true, если можно перенести начало события на время new_start_time без нарушения коректности.
    // false, иначе.
    // При любом исходе после завершения функции объект класса находится в том же
    // состоянии, что и до запуска. В методе происходит модификация объекта во время работы, поэтому нельзя
    // использовать для константных объектов.
    bool movable(const CEvent& from, size_t new_start_time);
    // Перенести начало события на новое время. Проверка коректности не производится.
    bool move(const CEvent& from, size_t new_start_time);

    friend class CTimeTableGeneratorSupporter;
    friend class CTimeTableBuilder;
    friend class CObjectiveFunction;

public:

    CTimeTable(const CTimeTable& timetable);
    CTimeTable& operator=(const CTimeTable& timetable);

    // Сгенерировать случайное корректное расписание
    void GenerateTimeTable();
    // Восстановить состояние объекта к начальному
    void RecoverTimeTable();
    // Произвести случайную перестановку случайных объектов без потери коректности
    void RandomSwap();
    // Произвести случайный перенос случайного события на случайное новое время
    void RandomMove();

    // Получить ссылку на событие группы group_name во время start_time
    const CEvent& GetEvent(std::string group_name, size_t start_time) const;
    // Получить ссылку на предмет subject_name
    const CSubject& GetSubject(std::string subject_name) const;

    // Генерация tex файлов для визуализации расписания для групп,
    // т.е. группа x время -> событие
    void GroupsScheduleTex(std::string filename) const;
    // Генерация tex файлов для визуализации расписания для учителей
    // т.е. учитель x время -> название предмета
    void TeachersScheduleTex(std::string filename) const;

};

// Вспомогательный класс для удобного создания объектов CTimeTable
// Содержит набор необходимых сеттеров, после вызовов которых объект CTimeTable
// создается вызовом метода Build.
//______________________________________________________________________________________________________________________
class CTimeTableBuilder {
private:

    std::map< std::string, CTeacher > teachers_;
    std::map< std::string, CCabinet > cabinets_;
    std::map< std::string, CGroup > groups_;
    std::map< std::string, CSubject > subjects_;

    size_t days_in_week_, lessons_in_day_;

public:

    void SetTimeTableTeachers(std::string teachers_filename);
    void SetTimeTableCabinets(std::string cabinets_filename);
    void SetTimeTableGroups(std::string groups_filename);
    void SetTimeTableSubjects(std::string subjects_filename);
    void SetTimeTableSize(size_t days_in_week, size_t lessons_in_day);

    CTimeTable Build();

};


#endif //TIMER_CTIMETABLE_H
