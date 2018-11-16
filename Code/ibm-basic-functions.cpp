/*
 *  ibm-basic-functions.c
 *
 *  Frequently used functions for IBM programming
 */

#include <stdio.h>
#include <stdlib.h>
#include "IBM.h"

extern double genrand_real1();

INDIVIDUAL *make_new_indiv(), *add_next_indiv(INDIVIDUAL*);
void delete_list(INDIVIDUAL*), display_list(INDIVIDUAL*);
void merge_list(INDIVIDUAL*, INDIVIDUAL*), survive(INDIVIDUAL*, double);
int number_of_indivs(INDIVIDUAL*);


int number_of_indivs(INDIVIDUAL *list)
{
	INDIVIDUAL *indiv;
    int count=0;
    
    for(indiv = list->next_indiv; indiv != NULL; indiv = indiv->next_indiv){
        count++;
    }

    return count;
}

INDIVIDUAL *make_new_indiv()
{
    INDIVIDUAL *new_indiv;
    
    new_indiv = (INDIVIDUAL *)malloc(sizeof(INDIVIDUAL));
	new_indiv->next_indiv = NULL;

    return new_indiv;
}

INDIVIDUAL *add_next_indiv(INDIVIDUAL *indiv)
{
    INDIVIDUAL *new_indiv;

    if(indiv->next_indiv != NULL) return NULL;

    new_indiv = make_new_indiv();
    indiv->next_indiv = new_indiv;

    return new_indiv;
}

void merge_list(INDIVIDUAL *list1, INDIVIDUAL *list2)
{
	INDIVIDUAL *indiv;
	
	for(indiv = list1; indiv->next_indiv != NULL; indiv = indiv->next_indiv);	

	indiv->next_indiv = list2->next_indiv;
	free(list2);	
}

void delete_list(INDIVIDUAL *list)
{
    INDIVIDUAL *indiv, *next_indiv;

    indiv = list;
    
    do{
        next_indiv = indiv->next_indiv;
        free(indiv);
    }while( (indiv = next_indiv) != NULL );
}

void display_list(INDIVIDUAL *list)
{
    int count = 0;
    INDIVIDUAL *indiv;
    
    printf("The list is \n");
    for( indiv = list->next_indiv; indiv != NULL; indiv = indiv->next_indiv){
        count++;
        printf("%f %f %d %f %f ", indiv->x, indiv->y, indiv->strategy, indiv->area, indiv->area_lost);
    }
    printf("The number of individuals is %d\n\n", count);
}

void survive(INDIVIDUAL *list, double rate)
{
	INDIVIDUAL *prev, *target, *next;
 
	prev = list;
	target = prev->next_indiv;
 
	while(target != NULL){
 
		if( genrand_real1() > rate){
 
			prev->next_indiv = target->next_indiv;
			next = target->next_indiv;
			free(target);
			target = next;
		}
		else{
			target = target->next_indiv;
			prev = prev->next_indiv;
		}
	}
}
