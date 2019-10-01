# school_timetable_generator

Software for automating routine scheduling work for schools.
-------------------------------

To use this program you should change input_folder_path - the folder consisting four files: 

    cabinets.txt - [room_name capacity available_time]
    groups.txt - [group_name number_of_stufents available_time]
    teachers.txt - [teacher_name available_time time_rating]
    subjects.txt - [subject_name id difficulty duration number_of_cabinets available_time
    number_of_teachers {teacher_name}* number_of_groups {group_name}* 
    number_of_feasible_cabinets {room_name}*]
    
Examples you can find in 10-11 or 8-11 folders.

You should also change output_folder_path - the folder where LaTex file will be saved.

--------

The criteria to build timetable is

  - The sooner the lesson the better
  - A penalty for the interval between lessons 
  - The farther the same class from each other the better 
  (for example, it’s good if the History is on Monday and Thursday, and not on Monday and Tuesday)
