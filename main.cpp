#include <iostream>
#include "CTeacher.h"
#include "CTeacher.cpp"
#include "CCabinet.h"
#include "CCabinet.cpp"
#include "CGroup.h"
#include "CGroup.cpp"
#include "CSubject.h"
#include "CSubject.cpp"
#include "CTimeTable.h"
#include "CTimeTable.cpp"
#include "CEvent.h"
#include "CEvent.cpp"
#include "CException.h"
#include "CException.cpp"
#include "CObjectiveFunction.cpp"
#include "CObjectiveFunction.h"
#include "CABCOptimizer.h"
#include "CABCOptimizer.cpp"
#include "Tests.h"
#include <unordered_set>

const int DAYS_IN_WEEK (5);
const int LESSONS_IN_DAY (7);

// Параметры алгоритма ABC
// https://github.com/Gregory06/TimeTableGenerator/blob/master/Theory/ArtificialBeeColonyAlgorithmforSolvingEducationalTimetablingProblems.pdf
const int POPULATION_SIZE (50);
const int CYCLES_NUMBER (4000);
const int IMPROVEMENT_LIMIT (750);

const std::string input_folder_path ("../8-11/");
const std::string output_folder_path ("/Users/greg/Desktop/Outputs/");

int main() {

    CTimeTableBuilder table_builder;

    table_builder.SetTimeTableTeachers(input_folder_path + "teachers.txt");
    table_builder.SetTimeTableGroups(input_folder_path + "groups.txt");
    table_builder.SetTimeTableCabinets(input_folder_path + "cabinets.txt");
    try {
        table_builder.SetTimeTableSubjects(input_folder_path + "subjects.txt");
    } catch (CException &ex) {
        std::cout << ex.GetMessage() << std::endl;
        return 0;
    }
    table_builder.SetTimeTableSize(DAYS_IN_WEEK, LESSONS_IN_DAY);

    CTimeTable table = table_builder.Build();

    CABCOptimizer optimizer(table, POPULATION_SIZE, CYCLES_NUMBER, IMPROVEMENT_LIMIT);
    optimizer.FindOptimal();

    optimizer.GetCurrentBestSolution().first.GroupsScheduleTex(output_folder_path + "GOutput.tex");
    optimizer.GetCurrentBestSolution().first.TeachersScheduleTex(output_folder_path + "TOutput.tex");

    return 0;
}