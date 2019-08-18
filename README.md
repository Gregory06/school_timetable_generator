# school_timetable_generator
Software for automating routine scheduling work for schools.

To use this program you should change input_folder_path - the folder consisting four files: 

cabinets.txt - [room_name capacity available_time]
groups.txt - [group_name number_of_stufents available_time]
teachers.txt - [teacher_name available_time time_rating]
subjects.txt - [subject_name id difficulty duration number_of_cabinets available_time number_of_teachers {teacher_name}* number_of_groups {group_name}* number_of_feasible_cabinets {room_name}*]

You should also change output_folder_path - the folder where LaTex file will be saved.
