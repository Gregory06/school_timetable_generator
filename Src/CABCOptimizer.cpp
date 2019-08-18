//
// Created by Gregory Postnikov on 2019-08-07.
//

#include "CABCOptimizer.h"

#include <random>



CABCOptimizer::CABCOptimizer( CTimeTable& timetable, size_t population_size,
                              size_t maximum_cycle_number, size_t single_source_limit )
        : current_best_solution_( std::make_pair(timetable, 0) ),
          population_size_(population_size),
          maximum_cycle_number_(maximum_cycle_number),
          single_source_limit_(single_source_limit) {

    solutions_.reserve(population_size_);

    current_best_solution_.first.GenerateTimeTable();
    current_best_solution_.second = cost_function_.Value(current_best_solution_.first);

    for (int i = 0; i < population_size_; i++)
        solutions_.emplace_back( std::make_pair(current_best_solution_.first, 0) );
}

void CABCOptimizer::memorizeBestSolution() {
    for (auto& [solution, changes_counter] : solutions_) {
        int current_cost = cost_function_.Value(solution);
        if ( current_best_solution_.second > current_cost ) {
            current_best_solution_ = std::make_pair(solution, current_cost);
        }
    }
}

void CABCOptimizer::sendEmploedBees() {
    srand( time(NULL) );
    for (auto& solution : solutions_) {
        sendBee(solution);
    }
}

void CABCOptimizer::sendOnlookerBees() {
    int values_sum = valuesSum();
    double previous_prob_sum(0), current_prob_sum(0);
    srand( time(NULL) );
    for (auto& solution : solutions_) {
        current_prob_sum = static_cast<double>( cost_function_.Value(solution.first) ) / values_sum * 1000;
        int choiser = rand() % 1000;
        if ( choiser < current_prob_sum / (1000 - previous_prob_sum) )
            sendBee(solution);
    }
}

void CABCOptimizer::sendScoutBees() {
    for (auto& [solution, changes_counter] : solutions_) {
        if ( changes_counter > single_source_limit_ ) {
            solution.RecoverTimeTable();
            solution.GenerateTimeTable();
            changes_counter = 0;
        }
    }
}

void CABCOptimizer::sendBee(std::pair<CTimeTable, size_t> &solution) {
    CTimeTable new_solution(solution.first);
    int choiser = rand() % 1000;
    if (choiser < 600) {
        new_solution.RandomSwap();
    } else {
        new_solution.RandomMove();
    }
    if ( cost_function_.Value(new_solution) >= cost_function_.Value(solution.first) )
        solution.second++;
    else {
        solution.first = new_solution;
        solution.second = 0;
    }
}

int CABCOptimizer::valuesSum() {
    int values_sum(0);
    for( const auto& [solution, change_counter] : solutions_ )
        values_sum += cost_function_.Value(solution);
    return values_sum;
}

void CABCOptimizer::FindOptimal() {

    for (int i = 0; i < maximum_cycle_number_; i++) {

        std::cout << "\r" << static_cast<double>(i)/maximum_cycle_number_*100 << "% completed.     Current best score: "
                  << current_best_solution_.second << std::flush;

        sendEmploedBees();
        sendOnlookerBees();
        sendScoutBees();

        memorizeBestSolution();
    }
}

auto CABCOptimizer::GetCurrentBestSolution() {
    return current_best_solution_;
}