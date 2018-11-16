/* Spatial territory game
 Hawk and Dove game in two dimentional space
 Individuals do not move. Only offspring disperse from parent.
 F Takasu
 Revised 20170414
*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "IBM.h"
#include "parameters.cpp"

// For random number generator
extern double genrand_real1(), gasdev(), poidev(double);
extern void init_genrand(unsigned long);

void initialize(), finish(), setup(), write_pos_to_file(FILE *, INDIVIDUAL *);
void play_games(INDIVIDUAL*), process();
int number_of_indivs_strategy(INDIVIDUAL*, int);
int number_of_offspring(double);
double min(double, double), torus(double), sqr(double), area_overlapped(double);
INDIVIDUAL *reproduce_young(INDIVIDUAL *list);

INDIVIDUAL *list_adult_head, *list_young_head;
long seed;
FILE *fp_position;

int main() 
{
	
    // Initialize rng and open files
	setup();

    // Set up initial state
    initialize();

    // Do it
    process();
    
    // Clear all before exit
	finish();
	
	return 0;
}

void setup()
{
    // Set up the seed for rng
	seed = (long)time(NULL);
	printf("The seed is %ld\n\n\n", seed);
	init_genrand(seed);

	#ifdef GAUSSIAN
		printf("Offspring dispersal is Gaussian\n");
	#else
		printf("Offspring dispersal is Complete Spatial Randomness\n");
	#endif
	
	fp_position = fopen("hd-territory-F=2-RANDOM-ONE-alpha=GAUSS","w");
}

void initialize()
{ 
	int i;
	INDIVIDUAL *indiv;
	
    list_adult_head = make_new_indiv(); /* create the roo of the list */
	indiv = list_adult_head;
	
	for(i = 0; i < 2; i++){ /* add two individuals */

		if(i < 1){
            indiv = add_next_indiv(indiv);
            indiv->strategy = HAWK;
            indiv->x = 0.25;
            indiv->y = 0.25;
            indiv->area = V;
            indiv->area_lost = 0.0;
            
		}else{
            indiv = add_next_indiv(indiv);
            indiv->strategy = DOVE;
            indiv->x = 0.75;
            indiv->y = 0.75;
            indiv->area = V;
            indiv->area_lost = 0.0;
		}
		
	}
} 

void process()
{
	INDIVIDUAL *list_young_head = NULL;
	int t;
	
	for(t = 0; t < STEP; t++){

		if( t%WRITE_INTV == 0 ){
			printf("Year %d, Pop size = %d, Hawks = %d, Doves = %d\n", t, number_of_indivs(list_adult_head),
				number_of_indivs_strategy(list_adult_head, HAWK), number_of_indivs_strategy(list_adult_head, DOVE));
			write_pos_to_file(fp_position, list_adult_head);	
			//display_list(list_adult_head);
		}
		
		// Let all individuals play games
        play_games(list_adult_head);
		
        // Let all individuals reproduce
		list_young_head = reproduce_young(list_adult_head);
        
        // Let all individuals survive to the next time step
		survive(list_adult_head, SH);
        
        // Merge survived individuals and new offspring
		merge_list(list_adult_head, list_young_head);

	}	/*end of t */
}

void play_games(INDIVIDUAL *list)
// Let all individuals play games in pairwise
{
	INDIVIDUAL *indiv, *indiv2;
	double dist_x, dist_y, distance, s;
	
	for(indiv = list->next_indiv; indiv != NULL; indiv = indiv->next_indiv){
		
		for(indiv2 = indiv->next_indiv; indiv2 != NULL; indiv2 = indiv2->next_indiv){
			
			/* Calculate the distance between the two individuals in the torus space */
			dist_x = min(fabs(indiv->x - indiv2->x), 1.0 - fabs(indiv->x - indiv2->x));
			dist_y = min(fabs(indiv->y - indiv2->y), 1.0 - fabs(indiv->y - indiv2->y));
			distance = sqrt( sqr(dist_x) + sqr(dist_y) );
			
            // A game is played between the two individuals, inidv and indiv2.
			if( distance < 2*R ){
				
                // Cacluate the area overlapped, s
				s = area_overlapped(distance);
				
				if(indiv->strategy == HAWK && indiv2->strategy == HAWK){
					if( genrand_real1() < 0.5 ){
						/* indiv wins the game */
						indiv2->area_lost += (1 + ALPHA)*s;
					}
					else{
						/* indiv2 wins the game */
						indiv->area_lost += (1 + ALPHA)*s;
					}
				}
				else if(indiv->strategy == HAWK && indiv2->strategy == DOVE){
					/* indiv wins */
					indiv2->area_lost += s;
				}
				else if(indiv->strategy == DOVE && indiv2->strategy == HAWK){
					/* indiv2 wins */
					indiv->area_lost += s;
				}
				else if(indiv->strategy == DOVE && indiv2->strategy == DOVE){
					indiv->area_lost += s/2;
					indiv2->area_lost += s/2;
				}
				
			} /* end of if distance < RADIUS_TERRITORY */
            
		} /* end of for indiv2 */
		
	} /* end of for indiv */

}

INDIVIDUAL *reproduce_young(INDIVIDUAL *list)
{
	INDIVIDUAL *young_head = make_new_indiv(); /* make a new list*/
	INDIVIDUAL *indiv, *young;
	double pos_x, pos_y, dx, dy;
    double payoff, average_num_offspring;
	int i, num;
 	
	young = young_head;

	for(indiv = list->next_indiv; indiv != NULL; indiv = indiv->next_indiv){
			 
        payoff = V - indiv->area_lost;
        if( payoff < 0 ) payoff = 0.0;
        average_num_offspring = REPRODUCTION_RATE*payoff/V;
        num = number_of_offspring( average_num_offspring );
        //printf("In reproduce_young, payoff = %f, average_num_offspring = %f\n", payoff, average_num_offspring);
        
        for( i = 0; i < num; i++){
            
#ifdef GAUSSIAN
            //angle = ran2(seed)*2*PI;
            dx = gasdev()*DIST_DISPERSAL;
            dy = gasdev()*DIST_DISPERSAL;
            pos_x = torus(indiv->x + dx);
            pos_y = torus(indiv->y + dy);
#else
            pos_x = genrand_real1()*1.0;
            pos_y = genrand_real1()*1.0;
#endif
            young = add_next_indiv(young);
            young->strategy = indiv->strategy;
            young->x = pos_x;
            young->y = pos_y;
            young->area = V;
            young->area_lost = 0.0;
            
        }
        
	}/* end of for indiv */

	return young_head;
}

int number_of_offspring(double x)
{
	int n;
	double p;
	
	if( x > 0.0 ){
	
		n = x;
		p = x - n;
		if( genrand_real1() < p )
			n++;
        
		//n = poidev(x, seed);
		
	} else {
		n = 0;
	}

	return n;
}

double torus(double iti)
// All position should be between 0.0 and 1.0
{
	while(iti < 0.0 || iti > 1.0){
		if( 0.0 > iti )
			iti = 1.0 + iti;
		else 
			iti = iti - 1.0;
	}
	
	return iti;
}

double area_overlapped(double x)
{
	double tmp;
	
	if( x < 2*R )
		tmp = PI*sqr(R) - x/2*sqrt(4*sqr(R) - sqr(x)) - 2*sqr(R)*atan(x/sqrt(4*sqr(R) - sqr(x)));
	else
		tmp = 0.0;
	
	return tmp;
}

int number_of_indivs_strategy(INDIVIDUAL *list, int strategy)
{
    INDIVIDUAL *indiv;
    int count = 0;

    for(indiv = list->next_indiv; indiv != NULL; indiv = indiv->next_indiv){
        if( indiv->strategy == strategy ) count++;
    }

    return count;
}

int number_of_hawks(INDIVIDUAL *list)
{
	INDIVIDUAL *indiv;
    int count=0;
    
    for(indiv = list->next_indiv; indiv != NULL; indiv = indiv->next_indiv){
        if( indiv->strategy == HAWK ) count++;
    }

    return count;
}

int number_of_doves(INDIVIDUAL *list)
{
	INDIVIDUAL *indiv;
    int count=0;
    
    for(indiv = list->next_indiv; indiv != NULL; indiv = indiv->next_indiv){
        if( indiv->strategy == DOVE ) count++;
    }

    return count;
}


void finish()
{
	delete_list(list_adult_head);
	fclose(fp_position);
}

void write_pos_to_file(FILE *fp, INDIVIDUAL *list)
{
	INDIVIDUAL *indiv;

	if( list->next_indiv != NULL)
		for(indiv = list->next_indiv; indiv != NULL; indiv = indiv->next_indiv){
			fprintf(fp, "%f %f %d %f ", indiv->x, indiv->y, indiv->strategy, V - indiv->area_lost);
		}
	else
		fprintf(fp, "%f %f %d %f ", 999.0, 999.0, 999, 999.0);
		
	fprintf(fp,"\n");
}

double sqr(double x)
{
	return x*x;
}

double min(double m, double n)
{
	if( m < n)
		return m;
	else
		return n;
}
