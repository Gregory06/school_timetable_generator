//
// Created by Gregory Postnikov on 2019-07-31.
//

#ifndef TIMER_TESTS_H
#define TIMER_TESTS_H

#include "ServiceFunctions.h"

void StartGroupsTimeTests(const std::string& test_folder_path) {

    CTimeTableBuilder table_builder;

    table_builder.SetTimeTableCabinets(test_folder_path + "cabinets.txt");
    table_builder.SetTimeTableTeachers(test_folder_path + "teachers.txt");
    table_builder.SetTimeTableGroups(test_folder_path + "groups.txt");

    try {
        table_builder.SetTimeTableSubjects(test_folder_path + "subjects.txt");
    } catch (CException& ex) {
        std::cout << ex.GetMessage() << std::endl;
    }

    table_builder.SetTimeTableSize(5, 7);

    CTimeTable table = table_builder.Build();

    std::cout << Int642Str(table.GetSubject("Русский").GetGroupAvailableTime() ) <<std::endl;


//    assert(table.GetSubject("Математика").GetGroupAvailableTime() == Str2Int64("1011101"));
    std::cout << "GROUPS  AVAILABLE TIME  TEST  OK" << std::endl;
}

void StartTeachersTimeTests(const std::string& test_folder_path) {

    CTimeTableBuilder table_builder;

    table_builder.SetTimeTableCabinets(test_folder_path + "cabinets.txt");
    table_builder.SetTimeTableTeachers(test_folder_path + "teachers.txt");
    table_builder.SetTimeTableGroups(test_folder_path + "groups.txt");

    try {
        table_builder.SetTimeTableSubjects(test_folder_path + "subjects.txt");
    } catch (CException& ex) {
        std::cout << ex.GetMessage() << std::endl;
    }

    table_builder.SetTimeTableSize(5, 7);

    CTimeTable table = table_builder.Build();

    std::cout << Int642Str(table.GetSubject("Русский").GetTeachersAvailableTime() ) <<std::endl;

//    assert(table.GetSubject("Математика").GetTeachersAvailableTime() == Str2Int64("1010101"));
    std::cout << "TEACHERS  AVAILABLE TIME  TEST  OK" << std::endl;
}

void StartSubjectTimeTests(const std::string& test_folder_path) {

    CTimeTableBuilder table_builder;

    table_builder.SetTimeTableCabinets(test_folder_path + "cabinets.txt");
    table_builder.SetTimeTableTeachers(test_folder_path + "teachers.txt");
    table_builder.SetTimeTableGroups(test_folder_path + "groups.txt");

    try {
        table_builder.SetTimeTableSubjects(test_folder_path + "subjects.txt");
    } catch (CException& ex) {
        std::cout << ex.GetMessage() << std::endl;
    }

    table_builder.SetTimeTableSize(5, 7);

    CTimeTable table = table_builder.Build();

    std::cout << Int642Str(table.GetSubject("Русский").GetAvailableTime() ) <<std::endl;

    assert(table.GetSubject("Русский").GetAvailableTime() == Str2Int64("1001101"));
    std::cout << "SUBJECT  AVAILABLE TIME  TEST  OK" << std::endl;

}

void StartSubjectStartTimeTests(const std::string& test_folder_path) {

    CTimeTableBuilder table_builder;

    table_builder.SetTimeTableCabinets(test_folder_path + "cabinets.txt");
    table_builder.SetTimeTableTeachers(test_folder_path + "teachers.txt");
    table_builder.SetTimeTableGroups(test_folder_path + "groups.txt");

    try {
        table_builder.SetTimeTableSubjects(test_folder_path + "subjects.txt");
    } catch (CException& ex) {
        std::cout << ex.GetMessage() << std::endl;
    }

    table_builder.SetTimeTableSize(5, 7);

    CTimeTable table = table_builder.Build();


    std::cout << Int642Str(table.GetSubject("Русский").GetAvailableStartTime(5, 7)) <<std::endl;
    assert(table.GetSubject("Русский").GetAvailableStartTime(5, 7) == Str2Int64("1001101"));
    std::cout << "SUBJECT  AVAILABLE START TIME  TEST  OK" << std::endl;
}

void ReserveReleaseTimeTests(const std::string& test_folder_path) {

    CTimeTableBuilder table_builder;

    table_builder.SetTimeTableCabinets(test_folder_path + "cabinets.txt");
    table_builder.SetTimeTableTeachers(test_folder_path + "teachers.txt");
    table_builder.SetTimeTableGroups(test_folder_path + "groups.txt");

    try {
        table_builder.SetTimeTableSubjects(test_folder_path + "subjects.txt");
    } catch (CException& ex) {
        std::cout << ex.GetMessage() << std::endl;
    }

    table_builder.SetTimeTableSize(5, 7);

    CTimeTable table = table_builder.Build();

    auto& groups = table.GetSubject("Математика").GetGroups();

    auto& group = *groups.begin();
    group->ReserveTime(3,3);

    assert(group->GetAvailableTime() == Str2Int64("1000111"));
    std::cout << "RESERVE  TIME  TEST  OK" << std::endl;

    group->ReleaseTime(3,3);

    assert(group->GetAvailableTime() == Str2Int64("1111111"));
    std::cout << "RELEASE  TIME  TEST  OK" << std::endl;
}

#endif //TIMER_TESTS_H
