/*
 * lab1b.c
 *
 *  Created on:
 *      Author:
 */

/* include helper functions for game */
#include "lifegame.h"

/* add whatever other includes here */

/* number of generations to evolve the world */
#define NUM_GENERATIONS 50

/* functions to implement -- can copy from Part A */

/* this function should set the state of all
   the cells in the next generation and call
   finalize_evolution() to update the current
   state of the world to the next generation */
void next_generation(void);

/* this function should return the state of the cell
   at (x,y) in the next generation, according to the
   rules of Conway's Game of Life (see handout) */
int get_next_state(int x, int y);

/* this function should calculate the number of alive
   neighbors of the cell at (x,y) */
int num_neighbors(int x, int y);

int main(int argc, char ** argv)
{
	int n;
	if(argc>1){
		initialize_world_from_file(*(argv+1));
	} else {
	initialize_world();
	}
	/* TODO: initialize the world */
	

	for (n = 0; n < NUM_GENERATIONS; n++)
		next_generation();

	/* TODO: output final world state */
	output_world();
	save_world_to_file("world.txt");
	return 0;
}

void next_generation(void) {
	/* TODO: for every cell, set the state in the next
	   generation according to the Game of Life rules

	   Hint: use get_next_state(x,y) */
	int w_height, w_width, x, y;
	w_height = get_world_height();
	w_width = get_world_width();
	for (x = 0; x < w_width; x++) {
		for (y = 0; y < w_height; y++) {
			set_cell_state(x,y,get_next_state(x,y));
		}
	}

	finalize_evolution(); /* called at end to finalize */
}

int get_next_state(int x, int y) {
	/* TODO: for the specified cell, compute the state in
	   the next generation using the rules

	   Use num_neighbors(x,y) to compute the number of live
	   neighbors */
	   int cur_neighbors = num_neighbors(x,y);
	   int cur_state = get_cell_state(x,y);
	   int next_state;
	   switch(cur_state) {
		   case ALIVE:
		   if(cur_neighbors < 2 || cur_neighbors > 3) {
			   next_state = DEAD;
		   } else {
			   next_state = ALIVE;
		   }
		   break;
		   case DEAD:
		   if (cur_neighbors == 3) {
			   next_state = ALIVE;
		   } else {
			   next_state = DEAD;
		   }
		   break;
	   } 
	return next_state;
}

int num_neighbors(int x, int y) {
	/* TODO: for the specified cell, return the number of
	   neighbors that are ALIVE

	   Use get_cell_state(x,y) */
int x_rot[8] = {0-1,-1,-1,0,1,1,1};
	int y_rot[8] = {1,1,0,-1,-1,-1,0,1};
	int i, count = 0;
	for(i = 0; i < 8; i++) {
		count += get_cell_state(x_rot[i]+x, y_rot[i]+y);   
		} 
	return count;
}
