//
// Created by Gregory Postnikov on 2019-08-07.
//

#ifndef TIMER_CABCOPTIMIZER_H
#define TIMER_CABCOPTIMIZER_H

#include "CTimeTable.h"
#include "CObjectiveFunction.h"

class CABCOptimizer {
protected:

    std::vector< std::pair<CTimeTable, size_t> > solutions_ {};
    std::pair<CTimeTable, size_t> current_best_solution_;

    size_t population_size_;
    size_t maximum_cycle_number_;
    size_t single_source_limit_;

    CObjectiveFunction cost_function_;

    void memorizeBestSolution();

    void sendEmploedBees();
    void sendOnlookerBees();
    void sendScoutBees();
    void sendBee(std::pair<CTimeTable, size_t>& solution);

    int valuesSum();

public:

    CABCOptimizer( CTimeTable& timetable, size_t population_size,
                   size_t maximum_cycle_number, size_t single_source_limit );

    void FindOptimal();
    auto GetCurrentBestSolution();

};


#endif //TIMER_CABCOPTIMIZER_H
