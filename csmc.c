#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>

// Semaphores
sem_t *access_control;
sem_t *coordinator_waiting;
sem_t *student_waiting;
sem_t *instructor_waiting;
sem_t *tutoring_session;

// Thread functions
void *student_function(void *param);
void *instructor_function(void *param);
void *coordinator_function(void *param);

// Structs
typedef struct student_struct {
    int urgency;
    int student_id;
    struct student_struct *next;
    int instructor_id;
    int selected;
} student_struct;

typedef struct {
    int instructor_id;
} instructor_struct;

// Global variables
int current_student_id = 1;
int current_instructor_id = 1;
int chairs_count;
int sessions_completed = 0;
int students_processed = 0;
int helps_needed;
int total_students;
int students_in_tutoring = 0;

student_struct *waiting_queue = NULL;
student_struct *priority_queue = NULL;

// Queue operations
void push(student_struct **queue, student_struct *new_student) {
    student_struct *temp = *queue;
    if (*queue == NULL) {
        *queue = new_student;
    } else {
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = new_student;
    }
}

student_struct *pop(student_struct **queue) {
    if (*queue == NULL) {
        return NULL;
    }
    student_struct *temp = *queue;
    *queue = (*queue)->next;
    temp->next = NULL;
    return temp;
}

// Main function
int main(int argc, char *argv[]) {
    // Semaphore initialization
    access_control = sem_open("/access_control_sem", O_CREAT, 0644, 1);
    coordinator_waiting = sem_open("/coordinator_waiting_sem", O_CREAT, 0644, 0);
    student_waiting = sem_open("/student_waiting_sem", O_CREAT, 0644, 0);
    instructor_waiting = sem_open("/instructor_waiting_sem", O_CREAT, 0644, 0);
    tutoring_session = sem_open("/tutoring_session_sem", O_CREAT, 0644, 0);

    total_students = atoi(argv[1]);
    int total_instructors = atoi(argv[2]);
    chairs_count = atoi(argv[3]);
    helps_needed = atoi(argv[4]);

    pthread_t coordinator_thread, *student_threads, *instructor_threads;
    student_threads = (pthread_t *)malloc(sizeof(pthread_t) * total_students);
    instructor_threads = (pthread_t *)malloc(sizeof(pthread_t) * total_instructors);

    // Creating student threads
    for (int i = 0; i < total_students; i++) {
        student_struct *stud = (student_struct *)malloc(sizeof(student_struct));
        stud->urgency = 0;
        stud->next = NULL;
        stud->instructor_id = 0;
        stud->selected = 0;
        stud->student_id = current_student_id++;
        pthread_create(&student_threads[i], NULL, student_function, (void *)stud);
    }

    // Creating coordinator thread
    pthread_create(&coordinator_thread, NULL, coordinator_function, NULL);

    // Creating instructor threads
    for (int i = 0; i < total_instructors; i++) {
        instructor_struct *instr = (instructor_struct *)malloc(sizeof(instructor_struct));
        instr->instructor_id = current_instructor_id++;
        pthread_create(&instructor_threads[i], NULL, instructor_function, (void *)instr);
    }

    // Joining threads
    for (int i = 0; i < total_students; i++) {
        pthread_join(student_threads[i], NULL);
    }

    pthread_cancel(coordinator_thread);

    for (int i = 0; i < total_instructors; i++) {
        pthread_cancel(instructor_threads[i]);
    }

    // Closing and unlinking semaphores
    sem_close(access_control);
    sem_unlink("/access_control_sem");
    sem_close(coordinator_waiting);
    sem_unlink("/coordinator_waiting_sem");
    sem_close(student_waiting);
    sem_unlink("/student_waiting_sem");
    sem_close(instructor_waiting);
    sem_unlink("/instructor_waiting_sem");
    sem_close(tutoring_session);
    sem_unlink("/tutoring_session_sem");

    return 0;
}

// Student thread function
void *student_function(void *param) {
    student_struct *stud = (student_struct *)param;
    while (stud->urgency < helps_needed) {
        usleep((rand() % 200) * 1000);

        sem_wait(access_control);
        if (chairs_count <= 0) {
            printf("Student %d found no empty chair. Will try again later.\n", stud->student_id);
            sem_post(access_control);
            continue;
        }

        chairs_count--;
        printf("Student %d takes a seat. Chairs available: %d.\n", stud->student_id, chairs_count);
        push(&waiting_queue, stud);
        sem_post(access_control);

        sem_post(coordinator_waiting);
        sem_wait(student_waiting);

        while (stud->selected == 0) {}

        chairs_count++;
        usleep(200);
        sem_wait(tutoring_session);
        printf("Student %d tutored by Instructor %d.\n", stud->student_id, stud->instructor_id);
        stud->urgency++;
        stud->selected = 0;
    }

    sem_wait(access_control);
    students_processed++;
    sem_post(access_control);

    pthread_exit(NULL);
}

// Coordinator thread function
void *coordinator_function(void *param) {
    while (students_processed != total_students) {
        sem_wait(coordinator_waiting);

        sem_post(student_waiting);

        sem_wait(access_control);
        student_struct *temp_stud = pop(&waiting_queue);
        if (temp_stud) {
            temp_stud->selected = 1;
            students_in_tutoring++;
            push(&priority_queue, temp_stud);
        }
        sem_post(access_control);

        sem_post(instructor_waiting);
    }

    pthread_exit(NULL);
}

// Instructor thread function
void *instructor_function(void *param) {
    instructor_struct *instr = (instructor_struct *)param;
    while (students_processed != total_students) {
        sem_wait(instructor_waiting);

        sem_wait(access_control);
        student_struct *temp_stud = pop(&priority_queue);
        if (temp_stud) {
            temp_stud->instructor_id = instr->instructor_id;
            sessions_completed++;
            students_in_tutoring--;
        }
        sem_post(access_control);

        usleep((rand() % 200) * 1000);
        sem_post(tutoring_session);
    }

    pthread_exit(NULL);
}

