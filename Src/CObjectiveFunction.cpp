//
// Created by Gregory Postnikov on 2019-08-04.
//

#include "CObjectiveFunction.h"

const int WINDOW_PENALTY = 100;
const int SAME_DAY_PENALTY = 100;

int CObjectiveFunction::Value(const CTimeTable &timetable) {

    int value(0);

    for (int day = 0; day < timetable.days_in_week_; day++) {
        for (const auto& [name, group] : timetable.time_table_) {
            bool window_flag (false);
            for (int lesson = static_cast<int>(timetable.lessons_in_day_)-1; lesson > -1; lesson--) {

                if ( group[day * timetable.lessons_in_day_ + lesson].IsActive() ) {
                    if (!window_flag) {
                        window_flag = true;
                    }
                    value += 2 * lesson * lesson;
                } else {
                    if (window_flag) {
                        value += WINDOW_PENALTY;
                    }
                }
            }
        }
    }

    for (const auto& [group_name, group] : timetable.event_linker_.GetLinkedEvents())
        for (const auto& [id, events_list] : group) {
            if (!events_list.size())
                break;
            int etalon_interval (timetable.days_in_week_ * timetable.lessons_in_day_ / events_list.size());
            int current_interval(0);
            auto prev_event (*events_list.begin());

            for (auto& event : events_list) {

                current_interval = ( event->GetStartTime() - prev_event->GetStartTime() );

                value += 7 * abs(current_interval-etalon_interval);

                if ( event->GetStartTime() / timetable.lessons_in_day_ ==
                     prev_event->GetStartTime() / timetable.lessons_in_day_ )
                    value += SAME_DAY_PENALTY;

                prev_event = event;
            }
        }

    return value;
}