// Here we define properties for individual

#include <stdio.h>

typedef struct individual INDIVIDUAL;

struct individual{
	int strategy;
	double x;
	double y;
	double area;
	double area_lost;
	struct individual *next_indiv;
};

INDIVIDUAL *make_new_indiv(), *add_next_indiv(INDIVIDUAL*);
void delete_list(INDIVIDUAL*), display_list(INDIVIDUAL*), write_to_file(FILE*, INDIVIDUAL*), merge_list(INDIVIDUAL*, INDIVIDUAL*);
void survive(INDIVIDUAL*, double);
int number_of_indivs(INDIVIDUAL*);
	
	
