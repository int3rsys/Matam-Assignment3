//
// Created by Ilya on 12/14/2017.
//

#ifndef ASSIGNMENT3_STUDENT_H
#define ASSIGNMENT3_STUDENT_H

#include <stdbool.h>
#include <stdio.h>
#include "set.h"

typedef struct Student_t *Student;
typedef enum StudentResult_t {
    STUDENT_SUCCESS,
    STUDENT_NULL_ARGUMENT,
    STUDENT_INVALID_ARGUMENT,
    STUDENT_OUT_OF_MEMORY,
    STUNDET_ALREADY_SENT_REQUEST,
    STUDENT_DIDNT_SEND_REQUEST,
    STUDENT_GRADE_DOES_NOT_EXIST
} StudentResult;

/* Creates a new student with given arguments
 * allocates new memory for relevant arguments
 * assert relevant arguments aren't NULL and id is valid
 * returns:
 * NULL if memory allocation failed
 * otherwise pointer to the new Student
 */
Student studentCreate(int id, char *first_name, char *last_name);

/* Copies the given student into a new Student
 * allocates a new memory for it
 * returns:
 * NULL is memory allocation failed or given student is NULL
 * otherwise returns a pointer to the new Student
 */
Student studentCopy(Student);

/* compare id's of two given students
 * returns:
 * positive if student1's id is bigger then student2's id
 * zero if both ids are equal
 * negative if student1's id is smaller then student2's id
 */
int studentCompare(Student student1, Student student2);

/* Sends a friend request to the given requested student
 * returns:
 * STUDENT_OUT_OF_MEMORY if memory problem occurred
 * STUDENT_NULL_ARGUMENT if one of the relevant arguments is NULL
 * STUNDET_ALREADY_SENT_REQUEST - if friend request was already sent
 * otherwise STUDENT_SUCCESS
 */
StudentResult studentSendFriendReuqest(Student,
                                       Student requested);

/* Checks if the given student sent a friend request to the requested student
 * returns:
 * STUDENT_NULL_ARGUMENT if one of the relevant arguments is NULL
 * otherwise STUDENT_SUCCESS
 */
StudentResult
studentIsSentFriendRequest(Student, Student requested, bool *result);

/* Removes a friend request which was sent by the given student to the requested student
 * returns:
 * STUDENT_NULL_ARGUMENT if one of the relevant arguments is NULL
 * STUDENT_DIDNT_SEND_REQUEST if no friend request was sent by the given student
 * otherwise STUDENT_SUCCESS
 */
StudentResult studentRemoveSentFriendRequest(Student, Student requested);

/* Adds a new grade to the Grades Sheet
 * returns:
 * STUDENT_OUT_OF_MEMORY if memory problem occurred
 * STUDENT_NULL_ARGUMENT if the given student sheet is null
 * STUDENT_INVALID_ARGUMENT if one of the given arguments is invalid
 * otherwise STUDENT_SUCCESS
 */
StudentResult
studentAddGrade(Student, int semester, int course_id, int points_x2,
                int grade_value);

/* Updates the given course's(latest taken) last(latest) grade with the given new grade
 * returns:
 * STUDENT_NULL_ARGUMENT if the given student is null
 * STUDENT_INVALID_ARGUMENT if one of the arguments is invalid
 * STUDENT_GRADE_DOES_NOT_EXIST if course's current grade doesn't exist
 * otherwise STUDENT_SUCCESS
 */
StudentResult studentUpdateLastGrade(Student, int course_id, int new_grade);

/* Removes the last(latest) grade of the given semester's course (also given)
 * returns:
 * STUDENT_INVALID_ARGUMENT if one of the arguments is invalid
 * STUDENT_NULL_ARGUMENT if the given student is null
 * otherwise STUDENT_SUCCESS
 */
StudentResult studentRemoveLastGrade(Student, int semester, int course_id);

/* Checks if the given student did the given course and stores the result in
 * a given pointer
 * returns:
 * STUDENT_NULL_ARGUMENT if the given student is null
 * STUDENT_INVALID_ARGUMENT if one of the arguments is invalid
 * otherwise STUDENT_SUCCESS
 */
StudentResult studentIsCourseDone(Student student, int course_id,
                                  bool *result);

/* Prints a full version (no grades restriction) of the given student's grades sheet
 * returns:
 * STUDENT_NULL_ARGUMENT if the given student is null
 * STUDENT_OUT_OF_MEMORY if memory problem occurred
 * otherwise STUDENT_SUCCESS
 */
StudentResult studentPrintFullSheet(FILE *output_channel, Student);

/* Prints a clean version (the effective grades) of the given student's grades sheet
 * returns:
 * STUDENT_NULL_ARGUMENT if the given student is null
 * STUDENT_OUT_OF_MEMORY if memory problem occurred
 * otherwise STUDENT_SUCCESS
 */
StudentResult studentPrintCleanSheet(FILE *output_channel, Student);

/* Prints a given amount of highest effective grades in the *clean version* grades sheet
 * returns:
 * STUDENT_NULL_ARGUMENT if the given student is null
 * STUDENT_INVALID_ARGUMENT if the argument is invalid
 * STUDENT_OUT_OF_MEMORY if memory problem occurred
 * otherwise STUDENT_SUCCESS
 */
StudentResult
studentPrintHighestGrades(FILE *output_channel, Student, int amount);

/* Prints a given amount of lowest effective grades in the *clean version* student's grades sheet
 * returns:
 * STUDENT_NULL_ARGUMENT if the student sheet is null
 * STUDENT_INVALID_ARGUMENT if the argument is invalid
 * STUDENT_OUT_OF_MEMORY if memory problem occurred
 * otherwise STUDENT_SUCCESS
 */
StudentResult
studentPrintLowestGrades(FILE *output_channel, Student, int amount);

/* Prints student's reference sources using a set of friends pointers.
 * returns:
 * STUDENT_NULL_ARGUMENT if one of the arguments is NULL
 * STUDENT_OUT_OF_MEMORY if memory problem occurred
 * STUDENT_INVALID_ARGUMENT if the argument is invalid
 * otherwise STUDENT_SUCCESS;
 */
StudentResult
studentPrintReferences(FILE *output_channel, Set friends,
                       int course_id, int amount);

/* Destroys all relevant memory associated to student
 */
void studentDestroy(Student);

#endif //ASSIGNMENT3_STUDENT_H
