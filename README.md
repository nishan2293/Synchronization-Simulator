# Seeking Tutor Problem - README

## Project Overview
This project, named "Seeking Tutor Problem," is designed to simulate a computer science department's mentoring center (csmc) using POSIX threads, mutex locks, and semaphores. The core concept revolves around coordinating students seeking tutoring, tutors providing assistance, and a coordinator managing the queue of students. The program models the interactions and prioritization based on various conditions.

## Problem Description
- The csmc lab has a coordinator, several tutors, and a waiting area with chairs.
- Students visit the lab for tutoring and wait in chairs if available; otherwise, they leave and return later.
- The coordinator prioritizes students based on their visit frequency – fewer visits mean higher priority.
- Tutors assist students with the highest priority and are notified by the coordinator of waiting students.

## Implementation Details
- **Threads**: Implement using POSIX threads for students, tutors, and the coordinator.
- **Synchronization**: Use mutex locks and semaphores for synchronization.
- **Dynamic Memory Allocation**: Allocate memory dynamically based on the command line arguments.
- **Command Line Arguments**: The program accepts the number of students, tutors, chairs, and help sessions as arguments. E.g., `csmc #students #tutors #chairs #help`.

## Output Specifications
- **Student Thread Output**: Indicate when a student takes a seat, finds no chair, or receives help.
- **Coordinator Thread Output**: Show when a student is added to the queue with their priority.
- **Tutor Thread Output**: Display information after tutoring a student.

## Error Handling and Debugging
- **Thread Limitation**: Check `.bashrc` for `ulimit -u 100` and adjust as necessary to allow more threads.
- **Unspecified Output**: Avoid any output not specified in the project description.
- **Error Messages**: Print all error messages on `stderr` to separate them from standard output.

## Implementation Hints
- **Shared Data Structures**: Utilize shared data structures for managing students and their priorities.
- **Semaphore Usage**: Employ semaphores for notifying the coordinator and waking up tutors.
- **Sleep Simulation**: Simulate programming and tutoring duration using sleep.

## Testing
- **Debugging Concurrent Programs**: Use assertions to validate the logic.
- **Assertions**: Add assertions to check the consistency of the program's logic, such as the number of students waiting or total sessions tutored.

