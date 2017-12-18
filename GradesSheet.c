//
// Created by Omer on 15/12/2017.
//

#include "headers/GradesSheet.h"
#include <stdlib.h>

#define GRADE_INSERT_LAST() \
new_semester = listCreate((CopyListElement) gradeCopyFunction,(FreeListElement) gradeFreeFunction); \
listInsertLast(new_semester, (ListElement) new_grade); \
listInsertLast(grades_sheet->sheet, (ListElement) new_semester); \

#define COMPARE_GRADE_FIELD($$field$$) \
if(grade1->$$field$$ > grade2->$$field$$){return 1;} \
else if(grade1->$$field$$ == grade2->$$field$$){return 0;} \
else{ return -1;}


struct GradesSheet_t {
    List sheet;
};

typedef struct Grade_t {
    int semester;
    int course_id;
    int points_x2;
    int grade_value;
} *Grade;

static ListElement semesterSheetCopy(ListElement semesterGradesList) {
    if ((List) semesterGradesList == NULL) return NULL;
    List new_sheet = listCopy((List) semesterGradesList);
    if (new_sheet == NULL) return NULL;
    //sheetCopyGrades(new_sheet, (List)semesterGradesList);
    listDestroy((List) semesterGradesList);
    return (ListElement) new_sheet;
}

static void semesterSheetDestroy(ListElement semesterGradesList) {
    listDestroy((List) semesterGradesList);
}

static Grade gradeCopyFunction(Grade grade) {
    if (grade == NULL) return NULL;
    Grade new_grade = malloc(sizeof(*new_grade));
    //TODO: check if previous grade needs to be freed
    if (new_grade == NULL) return NULL;
    new_grade->points_x2 = grade->points_x2;
    new_grade->grade_value = grade->grade_value;
    new_grade->semester = grade->semester;
    new_grade->course_id = grade->course_id;
    //free(grade);
    return new_grade;
}

static void gradeFreeFunction(Grade grade) {
    free(grade);
}

static Grade gradeCopy(int semester, int course_id,
                       int points_x2,
                       int grade_value) {
    Grade new_grade = malloc(sizeof(*new_grade));
    if (new_grade == NULL) return NULL;
    new_grade->course_id = course_id;
    new_grade->semester = semester;
    new_grade->grade_value = grade_value;
    new_grade->points_x2 = points_x2;
    return new_grade;
}

static int getSemesterLastGradeIndex(List semester_grade_list, int course_id) {
    int index = -1, currindex = 0;
    LIST_FOREACH(Grade, grade, semester_grade_list) {
        if (grade->course_id == course_id) {
            index = currindex;
        }
        currindex++;
    }
    return index;
}

static int getSemesterHighestGrade(List semester_grade_list, int course_id) {
    int max = -1, curr_max = -1;
    LIST_FOREACH(Grade, grade, semester_grade_list) {
        if (grade->course_id == course_id) {
            curr_max = grade->grade_value;
        }
        max = max > curr_max ? max : curr_max;
    }
    return max;
}


static int semesterGradeCompareFunction(Grade grade1, Grade grade2) {
    if (grade1->course_id > grade2->course_id) {
        return 1;
    } else if (grade1->course_id < grade2->course_id) {
        return 0;
    } else {
        return grade1->grade_value > grade2->grade_value ? 1 : 0;
    }
}

static void calcSemesterInfo(Grade current_grade, int *total_points,
                             int *failed_points, int *effective_point,
                             int *effective_grade_sum) {
    int total_p = 0, failed_p = 0, effective_p = 0, effective_g = 0;

    total_p += current_grade->points_x2;
    if (current_grade->grade_value < 55)
        failed_p += current_grade->points_x2;
    else effective_p += current_grade->points_x2;
    effective_g += effective_p * current_grade->grade_value;

    *total_points += total_p;
    *failed_points += failed_p;
    *effective_point += effective_p;
    *effective_grade_sum += effective_g;

}

static void
printSemesterInfo(FILE *output_stream, List semester, int *total_points_sheet,
                  int *failed_points_sheet, int *effective_point_sheet,
                  int *effective_grade_sum_sheet) {
    int total_points = 0, failed_points = 0, effective_points = 0, effective_grade_sum = 0;

    LIST_FOREACH(Grade, current_grade, semester) {
        mtmPrintGradeInfo(output_stream, current_grade->course_id,
                          current_grade->points_x2, current_grade->grade_value);
        calcSemesterInfo(current_grade, &total_points, &failed_points,
                         &effective_points, &effective_grade_sum);
    }
    mtmPrintSemesterInfo(output_stream,
                         ((Grade) listGetFirst(semester))->course_id,
                         total_points, failed_points, effective_points,
                         effective_grade_sum);

    *total_points_sheet += total_points;
    *failed_points_sheet += failed_points;
    *effective_point_sheet += effective_points;
    *effective_grade_sum_sheet = effective_grade_sum;

}


static bool isSportCourse(int course_id) {
    if (course_id >= 390000 && course_id <= 399999) return true;
    return false;
}

static bool filterOutSportCoursesFunction(Grade grade, ListSortKey key) {
    int *counted = &((int *) key)[grade->course_id];
    if (!isSportCourse(grade->course_id)) {
        return true;
    } else if (grade->grade_value < 55) {
        (*counted)--;
        return false;
    } else if (*(counted) > 1) {
        (*counted)--;
        return false;
    } else {
        (*counted)--;
        return true;
    }
}

static bool filterCourse(Grade grade, ListFilterKey key) {
    int *counted = &((int *) key)[grade->course_id];
    if (grade->grade_value < 55) {
        (*counted)--;
        return false;
    } else if (*(counted) > 1) {
        (*counted)--;
        return false;
    } else {
        (*counted)--;
        return true;
    }
}

static List listConcatenator(GradesSheet grade_sheet) {
    List concatanated_list = listCreate((CopyListElement) gradeCopyFunction,
                                        (FreeListElement) gradeFreeFunction);
    LIST_FOREACH(List, semester_grade_list, grade_sheet->sheet) {
        LIST_FOREACH(Grade, current_grade, semester_grade_list) {
            listInsertLast(concatanated_list, current_grade);
            //histogram[current_grade->course_id]++; TODO:remove
        }
    }
    return concatanated_list;
}

static void printSemesterInfoClean(FILE *output_stream, List grade_sheet,
                                   int *effective_point_sheet,
                                   int *effective_grade_sum_sheet) {
    int effective_p = 0, effective_g = 0;
    LIST_FOREACH(Grade, current_grade, grade_sheet) {
        mtmPrintGradeInfo(output_stream, current_grade->course_id,
                          current_grade->points_x2, current_grade->grade_value);
        effective_p += current_grade->points_x2;
        effective_g += effective_p * current_grade->grade_value;
    }

    *effective_point_sheet += effective_p;
    *effective_grade_sum_sheet += effective_g;

}

static void filterOutSportCourses(GradesSheet grades_sheet,
                                  GradesSheet sport_filtered_sheet, int* histogram) {
    List temp_filtered_list;

    LIST_FOREACH(List, semester_grades_list, grades_sheet->sheet) {
        LIST_FOREACH(Grade, current_grade, semester_grades_list) {
            histogram[current_grade->course_id]++;
        }
        //counter = countSportCoursesInSemester(semester_grades_list);
        //listGetFirst(semester_grades_list);
        temp_filtered_list = listFilter(semester_grades_list,
                                        (FilterListElement) filterOutSportCoursesFunction,
                                        (ListSortKey) histogram);
        listInsertLast(sport_filtered_sheet->sheet, temp_filtered_list);
    }
}

static List generateCleanList(GradesSheet grades_sheet) {
    List clean_list;
    int *histogram = calloc(1000000, sizeof(int));
    GradesSheet sport_filtered_sheet = sheetCreate();
    filterOutSportCourses(grades_sheet, sport_filtered_sheet, (ListFilterKey)histogram);
    List joined_grade_sheet = listConcatenator(sport_filtered_sheet);

    clean_list = listFilter(joined_grade_sheet,
                            (FilterListElement) filterCourse,
                            (ListFilterKey) histogram);
    free(histogram);
    listDestroy(joined_grade_sheet);
    sheetDestroy(sport_filtered_sheet);
    return clean_list;
}

static int compareCourseId(Grade grade1, Grade grade2) {
    COMPARE_GRADE_FIELD(course_id);
}

static int compareGradeValue(Grade grade1, Grade grade2) {
    COMPARE_GRADE_FIELD(grade_value);
}

static int compareSemester(Grade grade1, Grade grade2) {
    COMPARE_GRADE_FIELD(semester);
}

static int semeserGradesSortFunction(Grade grade1, Grade grade2, ListSortKey key) {
    int condition = *(int*)key;
    if (compareGradeValue(grade1, grade2) == 1) {
        return condition;
    } else if (compareGradeValue(grade1, grade2) == 0) {
        if (compareSemester(grade1, grade2) == 1) {
            return condition;
        } else if (compareSemester(grade1, grade2) == 0) {
            if (compareCourseId(grade1, grade2) == 1) {
                return condition;
            } else {
                return !condition;
            }
        } else {
            return !condition;
        }
    } else {
        return !condition;
    }
}

GradesSheet sheetCreate() {
    GradesSheet new_sheet = malloc(sizeof(*new_sheet));
    new_sheet->sheet = listCreate((CopyListElement) semesterSheetCopy,
                                  (FreeListElement) semesterSheetDestroy);
    if (new_sheet->sheet == NULL) return NULL;
    return new_sheet;
}

GradesSheet sheetCopy(GradesSheet sheet_to_be_copied) {
    if (sheet_to_be_copied == NULL || sheet_to_be_copied->sheet == NULL)
        return NULL;
    GradesSheet new_sheet = sheetCreate();
    if (new_sheet == NULL) return NULL;
    LIST_FOREACH(List, semesterGradesList, sheet_to_be_copied->sheet) {
        List copied_semester_grades_list = listCopy(semesterGradesList);
        if (copied_semester_grades_list == NULL) {
            listDestroy(new_sheet->sheet);
            return NULL;
        }
        listInsertLast(new_sheet->sheet, copied_semester_grades_list);
    }
    return new_sheet;
}


SheetResult
sheetAddGrade(GradesSheet grades_sheet, int semester, int course_id,
              int points_x2,
              int grade_value) {

    if (grades_sheet == NULL || grades_sheet->sheet == NULL)
        return SHEET_NULL_ARGUMENT;
    if (semester <= 0 || course_id <= 0 || course_id > 1000000 ||
        grade_value < 0 || grade_value > 100)
        return SHEET_INVALID_ARGUMENT;
    //points_x2 is already validated beforehand

    Grade new_grade = gradeCopy(semester, course_id, points_x2, grade_value);
    List new_semester;
    int grade_inserted = 0;
    //TODO:change all ListElement to regular list
    LIST_FOREACH(ListElement, semester_grades_list, grades_sheet->sheet) {
        Grade first_grade_in_semester = (Grade) listGetFirst(
                (List) semester_grades_list);
        if (first_grade_in_semester->semester == new_grade->semester) {
            listInsertLast(semester_grades_list, (ListElement) new_grade);
            grade_inserted = 1;
            break;
        }
    }
    if (!grade_inserted) {
        GRADE_INSERT_LAST();
    }
    free(new_grade);
    return SHEET_SUCCESS;
}

SheetResult
sheetRemoveLastGrade(GradesSheet grades_sheet, int semester, int course_id) {
    if (grades_sheet == NULL || grades_sheet->sheet == NULL)
        return SHEET_NULL_ARGUMENT;
    if (semester <= 0 || course_id <= 0 || course_id > 1000000)
        return SHEET_INVALID_ARGUMENT;
    List semester_grades;
    LIST_FOREACH(ListElement, semester_grades_list, grades_sheet->sheet) {
        Grade first_grade_in_semester = (Grade) listGetFirst(
                (List) semester_grades_list);
        if (first_grade_in_semester != NULL &&
            first_grade_in_semester->semester == semester) {
            semester_grades = (List) semester_grades_list;
            break;
        }
    }
    int last_grade_index = getSemesterLastGradeIndex(semester_grades,
                                                     course_id);
    if (last_grade_index != -1) {
        LIST_FOREACH(Grade, grade_iterator, semester_grades) {
            if (!last_grade_index) {
                listRemoveCurrent(semester_grades);
                break;
            }
            last_grade_index--;
        }
    }
    return SHEET_SUCCESS;
}

SheetResult
sheetUpdateLastGrade(GradesSheet grades_sheet, int course_id, int new_grade) {
    if (grades_sheet == NULL || grades_sheet->sheet == NULL)
        return SHEET_NULL_ARGUMENT;
    if (course_id <= 0 || course_id > 1000000 ||
        new_grade < 0 || new_grade > 100)
        return SHEET_INVALID_ARGUMENT;
    List semester_grades;
    int current_index = -1, lastindex = -1;
    LIST_FOREACH(ListElement, semester_grades_list, grades_sheet->sheet) {
        current_index = getSemesterLastGradeIndex(semester_grades_list,
                                                  course_id);
        if (current_index != -1) semester_grades = semester_grades_list;
        current_index = current_index > lastindex ? current_index : lastindex;
        lastindex = current_index;
    }
    if (current_index != -1) {
        LIST_FOREACH(Grade, grade_iterator, semester_grades) {
            if (!current_index) {
                grade_iterator->grade_value = new_grade;
                break;
            }
            current_index--;
        }
        return SHEET_SUCCESS;
    }
    return SHEET_GRADE_DOES_NOT_EXIST;
}

SheetResult
sheetHighestLastGrade(GradesSheet grades_sheet, int course_id, int *result) {
    if (grades_sheet == NULL || grades_sheet->sheet == NULL)
        return SHEET_NULL_ARGUMENT;
    if (course_id <= 0 || course_id > 1000000)
        return SHEET_INVALID_ARGUMENT;
    if (result == NULL) return SHEET_NULL_ARGUMENT;
    int curr_max = -1, max = -1;
    LIST_FOREACH(ListElement, semester_grades_list, grades_sheet->sheet) {
        curr_max = getSemesterHighestGrade(semester_grades_list,
                                           course_id);
        max = max > curr_max ? max : curr_max;
    }
    if (max == -1) return SHEET_GRADE_DOES_NOT_EXIST;
    *result = max;
    return SHEET_SUCCESS;

}

SheetResult sheetPrintFull(FILE *output_channel, GradesSheet grades_sheet) {
    if (grades_sheet == NULL || grades_sheet->sheet == NULL ||
        output_channel == NULL)
        return SHEET_NULL_ARGUMENT;

    GradesSheet sorted_sheet = sheetCopy(grades_sheet);
    int key = 0;
    LIST_FOREACH(List, semester_grades_list, sorted_sheet->sheet) {
        listSort(semester_grades_list,
                 (CompareListElements) semesterGradeCompareFunction,
                 (ListSortKey) &key);
    }

    int total_points_sheet = 0, failed_points_sheet = 0, effective_point_sheet = 0, effective_grade_sum_sheet = 0;
    LIST_FOREACH(List, semester_grades_list, sorted_sheet->sheet) {
        printSemesterInfo(output_channel, semester_grades_list,
                          &total_points_sheet,
                          &failed_points_sheet, &effective_point_sheet,
                          &effective_grade_sum_sheet);

    }
    mtmPrintSummary(output_channel, total_points_sheet, failed_points_sheet,
                    effective_point_sheet, effective_grade_sum_sheet);

    sheetDestroy(sorted_sheet);

    return SHEET_SUCCESS;
}

SheetResult sheetPrintClean(FILE *output_channel, GradesSheet grades_sheet) {
    if (grades_sheet == NULL || grades_sheet->sheet == NULL ||
        output_channel == NULL)
        return SHEET_NULL_ARGUMENT;

    int key = 0;

    List clean_grades_list = generateCleanList(grades_sheet);
    listSort(clean_grades_list,
             (CompareListElements) semesterGradeCompareFunction,
             (ListSortKey) &key);
    int effective_point_sheet = 0, effective_grade_sum_sheet = 0;
    printSemesterInfoClean(output_channel, clean_grades_list,
                           &effective_point_sheet,
                           &effective_grade_sum_sheet);

    mtmPrintCleanSummary(output_channel, effective_point_sheet,
                         effective_grade_sum_sheet);

    listDestroy(clean_grades_list);
    return SHEET_SUCCESS;

}

SheetResult
sheetPrintHighestGrades(FILE *output_channel, GradesSheet grades_sheet,
                        int amount) {
    if (grades_sheet == NULL || grades_sheet->sheet == NULL ||
        output_channel == NULL)
        return SHEET_NULL_ARGUMENT;
    if (amount <= 0) return SHEET_INVALID_ARGUMENT;
    int key = 0, counter=amount, list_size;
    List clean_grades_list = generateCleanList(grades_sheet);

    list_size = listGetSize(clean_grades_list);
    counter = list_size>counter?counter:list_size;

    listSort(clean_grades_list,
             (CompareListElements) semeserGradesSortFunction,
             (ListSortKey) &key);

    LIST_FOREACH(Grade, current_grade, clean_grades_list) {
        if(counter == 0) break;
        mtmPrintGradeInfo(output_channel, current_grade->course_id,
                          current_grade->points_x2,
                          current_grade->grade_value);
        counter--;

    }
    listDestroy(clean_grades_list);
    return SHEET_SUCCESS;
}

SheetResult sheetPrintLowestGrades(FILE* output_channel, GradesSheet grades_sheet, int amount){
    if (grades_sheet == NULL || grades_sheet->sheet == NULL ||
        output_channel == NULL)
        return SHEET_NULL_ARGUMENT;
    if (amount <= 0) return SHEET_INVALID_ARGUMENT;
    int key = 1, counter=amount, list_size;
    List clean_grades_list = generateCleanList(grades_sheet);

    list_size = listGetSize(clean_grades_list);
    counter = list_size > counter ? counter:list_size;
    listSort(clean_grades_list,
             (CompareListElements) semeserGradesSortFunction,
             (ListSortKey) &key);

    LIST_FOREACH(Grade, current_grade, clean_grades_list) {
        if(counter==0) break;
        mtmPrintGradeInfo(output_channel, current_grade->course_id,
                          current_grade->points_x2,
                          current_grade->grade_value);
        counter--;
    }
    listDestroy(clean_grades_list);
    return SHEET_SUCCESS;
}

void sheetDestroy(GradesSheet grades_sheet) {
    listDestroy(grades_sheet->sheet);
    free(grades_sheet);
}

void printSheet(GradesSheet gsheet) {
    printf("\n");
    LIST_FOREACH(ListElement, semester_list, gsheet->sheet) {
        LIST_FOREACH(Grade, semester_grade, semester_list) {
            Grade current = semester_grade;
            printf("Semester:%d, CourseID:%d, Points:%d, Grade:%d\n",
                   current->semester, current->course_id, current->points_x2,
                   current->grade_value);
        }
    }
}

// --------- functions graveyard ---------

/*static int countSportCoursesInSemester(List semester) {
    int cnt = 0;
    LIST_FOREACH(Grade, current_grade, semester) {
        if (current_grade->course_id >= 390000 &&
            current_grade->course_id <= 399999 &&
            current_grade->grade_value >= 55)
            cnt++;
    }
    return cnt;
}*/
/*static int semesterGradesSortFunctionRouter(int route,Grade grade1, Grade grade2){
    if(route == 1) return semeserGradesSortFunction(route, grade1, grade2);
    else return semeserGradesSortFunction(route, grade1, grade2);
}*/

/*static int semesterCompareFunction(List semester1, List semester2) {
    Grade grade1, grade2;
    grade1 = listGetFirst(semester1);
    grade2 = listGetFirst(semester2);
    if (grade1->course_id > grade2->course_id) return 1;
    else return 0;
}*/
/*
static void sheetDestroyGrades(List list) {
    LIST_FOREACH(ListElement, grades_list, list) {
        listDestroy(grades_list);
    }
}
*/

/*static char* listCopyElement(char* semester_id){
    assert(semester_id != NULL);
    ListElement new_semester = malloc(sizeof(semester_id));
    if(new_semester == NULL) return NULL;
    return new_semester;
}

static void listFreeElement(char* semester_id){
    assert(semester_id != NULL);
    free(semester_id);
}*/

/*static void sheetCopyGrades(List new_list, List list_copied_from) {
    LIST_FOREACH(ListElement, semester_grades, list_copied_from) {
        List copied_grades_list = listCopy(semester_grades);
        if (copied_grades_list == NULL) listDestroy(new_list);
        listInsertLast(new_list, copied_grades_list);
    }
}*/