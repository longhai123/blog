#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#define container_of(ptr, type, member) ({ \
    const typeof( ((type *)0)->member ) *__mptr = (ptr); \
    (type *)( (char *)__mptr - offsetof(type,member) );})

/* The sutdent structure definition */
typedef struct student {
    char name[16];
    int id;
    char addr[64];
    char phone_num[16];
} STUDENT, *STUDENT_PTR;

void print_id(char *phone_numP)
{
    STUDENT_PTR stdP = NULL;

    /* Get the structure pointer by giving the mmeber 
     * address of 'phone_numP'.
     */
    stdP = container_of(phone_numP, STUDENT, phone_num);

    printf("Student: %s, His ID: %d\n", stdP->name, stdP->id);

    return;
}

int main(void)
{
    STUDENT_PTR stdP = NULL;

    if ((stdP = (STUDENT_PTR)malloc(sizeof(STUDENT))) == NULL) {
        perror("malloc error");
        return 1;
    }

    /* assign the value for each memnber */
    strncpy(stdP->name, "Longhai", sizeof(stdP->name));
    stdP->id = 123456;
    strncpy(stdP->addr, "Road 1", sizeof(stdP->addr));
    strncpy(stdP->phone_num, "11111111", sizeof(stdP->phone_num));

    print_id(stdP->phone_num);

    return 0;
}
