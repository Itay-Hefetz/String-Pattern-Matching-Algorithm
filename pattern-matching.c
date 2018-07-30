#include "pattern-matching.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define SUCCESS 0
#define ERROR -1
#define ERROR_NULL 1
/* Declarations of functions*/
pm_state_t * create_state (pm_int_t id, pm_int_t depth);
void free_DFS(pm_state_t*);
void create_match(slist_node_t* temp, slist_t* matches_list, int i, pm_state_t *state);
int init_state(pm_state_t * new_state,pm_int_t id, pm_int_t depth);
int is_null (void* l);
int check_fsm(pm_state_t *root);
// This function initializing the pm
int pm_init(pm_t * pm){
	if(is_null (pm))
		return ERROR;
	pm->zerostate = (pm_state_t*)calloc(1,(sizeof(pm_state_t)));
	if(is_null (pm->zerostate))
		return ERROR;
	if(init_state(pm->zerostate, 0, 0) == ERROR)
		return ERROR;
	pm->zerostate-> fail = NULL;
	pm->newstate = 1;
	return SUCCESS;
}
/* Set a transition arrow from this from_state, via a symbol, to a
   to_state. will be used in the pm_addstring and pm_makeFSM functions.
   Returns 0 on success, -1 on failure.*/
int pm_goto_set(pm_state_t *from_state, unsigned char symbol, pm_state_t *to_state){
	if (is_null (from_state) || is_null (to_state))
			return ERROR;
	pm_labeled_edge_t* edge = (pm_labeled_edge_t*)calloc(1,(sizeof(pm_labeled_edge_t)));
	if(is_null (edge))
		return ERROR;
	printf("%d -> %c -> %d\n",from_state->id, (char)symbol, to_state->id );
	edge-> label = symbol;
	edge-> state = to_state;
	if (slist_append(from_state->_transitions, edge) == ERROR)
		return ERROR;
	return SUCCESS;
}
/* Returns the transition state.  If no such state exists, returns NULL. 
   will be used in pm_addstring, pm_makeFSM, pm_fsm_search, pm_destroy functions. */
pm_state_t* pm_goto_get(pm_state_t *state, unsigned char symbol){
	if (is_null (state))
			return NULL;
	slist_node_t* temp = slist_head(state->_transitions);
	while(temp != NULL){
		pm_labeled_edge_t* edge =slist_data(temp);
		if(edge->label==symbol)
			return edge->state;
		temp = slist_next(temp);
	}
	return NULL;
}
//  --------- Add String --------- // 
/* Adds a new string to the fsm, given that the string is of length n. 
   Returns 0 on success, -1 on failure.*/
int pm_addstring(pm_t * pm, unsigned char *word, size_t n){
	if (is_null (pm) || is_null (word)|| n == 0 || strlen((char*)word) != n ||is_null(pm->zerostate))
		return ERROR;
	pm_state_t* temp_state = pm -> zerostate;
	pm_state_t* tempo_state;
	pm_int_t i=0;
	for (; i<n; i++)
	{
		tempo_state = pm_goto_get(temp_state, *(word+i));
		if (is_null(tempo_state))
			break;
		temp_state = tempo_state;
			
	}
	pm_state_t* new_state;
	if (i == n && tempo_state != NULL){ //the word is exists in the automat
		if(is_null (slist_head(tempo_state->output)))// if is not a final state
			if(slist_append((tempo_state->output),word) == ERROR)
				return ERROR;
		return SUCCESS;
	}
	for (; i<n; i++){
		new_state = create_state (pm->newstate, (temp_state->depth)+1); //create state
		if(pm_goto_set(temp_state, *(word+i), new_state) == ERROR)	
			return ERROR;
		pm->newstate++;
		temp_state = pm_goto_get(temp_state, *(word+i));
		
	}
	if (is_null (slist_head(new_state->output))){ //if is not a final state
		if(slist_append((new_state->output),word) == ERROR)
			return ERROR;
	}
	return SUCCESS;
}
/* Finalizes construction by setting up the failrue transitions, as
   well as the goto transitions of the zerostate. 
   Returns 0 on success, -1 on failure.*/
int pm_makeFSM(pm_t * pm){
	if(is_null (pm) || is_null (pm->zerostate)|| is_null(pm-> zerostate-> _transitions) ||
		slist_size(pm->zerostate->_transitions) == 0)
		return ERROR;
	slist_t* queue = (slist_t*)calloc(1,(sizeof(slist_t))); // empty queue
	if (is_null (queue))
		return ERROR;
	slist_init(queue);
	int i;
	slist_node_t* edge = (pm->zerostate->slist_head(_transitions));
	pm_state_t* state;

	while(edge != NULL){// insert to the queue all the states with depth 1
		state = ((pm_labeled_edge_t*)(slist_data(edge)))-> state;
		if (slist_append(queue ,state) == ERROR)
			return ERROR;
		state->fail = pm->zerostate;
		edge = slist_next(edge);
	}
	while (slist_head(queue) != NULL){ //BFS
		pm_state_t* r = (pm_state_t*)(slist_pop_first(queue));
		edge = r->slist_head(_transitions);
		for(i=0; i< slist_size(r->_transitions); i++){
			pm_labeled_edge_t* next_edge = (pm_labeled_edge_t*)(slist_data(edge));
			pm_state_t* s = next_edge-> state;
			if (slist_append(queue ,s) == ERROR)
				return ERROR;
			pm_state_t* state_fail = r-> fail;
			do {
				if(is_null (state_fail)){ //fail = zerostate
					s->fail = pm->zerostate;
					break;
				}
				s->fail = pm_goto_get(state_fail, next_edge->label);
				state_fail = state_fail->fail;
			}while ( is_null (s->fail) );
			if (slist_append_list(s->output, s->fail->output) == ERROR)
				return ERROR;
			printf("Setting f(%d) = %d\n", s->id, s->fail->id);
			edge = slist_next(edge);
		}
	}
	free(queue);
	queue=NULL;
	return SUCCESS;
}
/* Search for matches in a string of size n in the FSM. 
   if there are no matches return empty list */
slist_t* pm_fsm_search(pm_state_t *root,unsigned char *text,size_t length){
	if(is_null (root) || is_null (text) || is_null(root->_transitions) ||
	 is_null(slist_head(root->_transitions))|| check_fsm(root) == ERROR)
		return NULL;
	slist_t* matches_list = (slist_t*)calloc(1,(sizeof(slist_t)));
	if(is_null (matches_list))
		return NULL;
	slist_init(matches_list);
	pm_state_t *state = root;
	int i,j;
	for (i=0; i<length; i++){
		while(state != NULL && pm_goto_get(state,*(text+i)) == NULL){
			if(check_fsm(state) == ERROR)
				return matches_list;
			state = state->fail;
		}
		if (is_null (state)){
			state = root;
			continue;
		}
		state = pm_goto_get(state,*(text+i));
		if (slist_size(state->output) > 0){
			slist_node_t* temp = slist_head(state->output);
			for(j=0; j<slist_size(state->output); j++){
				create_match(temp,matches_list,i, state);
				temp = slist_next(temp);
			}
		}
	}
	return matches_list;
}
/* Destroys the fsm, deallocating memory. */
void pm_destroy(pm_t *pm){
	if (is_null (pm) || is_null (pm->zerostate) || is_null(pm->zerostate-> _transitions))
		return;
	free_DFS(pm->zerostate);
}
/* PRIVATE FUNCTIONS */
//  --------- Create State --------- //
// create state to pm
pm_state_t * create_state (pm_int_t id, pm_int_t depth){
	pm_state_t * new_state = (pm_state_t*)calloc(1,(sizeof(pm_state_t)));
	if (is_null (new_state))
		return NULL;
	printf("Allocating state %d\n", id);
	if(init_state(new_state, id, depth) == ERROR)
		return NULL;
	return new_state;
}
//  --------- Init State --------- //
/* This function initializing a state 
   Returns 0 on success, -1 on failure*/
int init_state(pm_state_t * new_state,pm_int_t id, pm_int_t depth){
	new_state->id = id;
	new_state->depth = depth;
	new_state-> output = (slist_t*)calloc(1,(sizeof(slist_t)));
	new_state-> _transitions=(slist_t*)calloc(1,(sizeof(slist_t)));
	if(is_null(new_state-> output) || is_null(new_state-> _transitions))
		return ERROR;
	slist_init(new_state-> output);
	slist_init(new_state-> _transitions);

	return SUCCESS;
	}
//  --------- Create Match --------- //
// create node for the matches list
void create_match(slist_node_t* temp, slist_t* matches_list,int i, pm_state_t *state){
	pm_match_t* data = (pm_match_t*)calloc(1,(sizeof(pm_match_t)));
	if (is_null(data))
		return;
	data-> pattern = (char*)(temp->data);
	data->start_pos = i - strlen(data->pattern)+1;
	data->end_pos = i;
	data->fstate = state;
	slist_append(matches_list, data);
	printf("Pattern: %s, start at: %d, ends at: %d, last state = %d\n",
	 data->pattern, data->start_pos, i, data->fstate->id);
	}
//  --------- Free DFS --------- //
// clean memory with DFS algorithm
void free_DFS(pm_state_t* v){
	slist_destroy(v->output, SLIST_LEAVE_DATA);
	free(v->output);
	v->output = NULL;
	int i;
	slist_node_t* temp = slist_head(v->_transitions);
	for (i=0; i<slist_size(v->_transitions); i++){
		pm_labeled_edge_t* edge = (pm_labeled_edge_t*) slist_data(temp);
		pm_state_t* u = edge-> state;
		if((u->output)!=NULL)
			free_DFS(u);
		temp = slist_next(temp);
	}
	slist_destroy(v->_transitions, SLIST_FREE_DATA);
	free(v->_transitions);
	v->_transitions = NULL;
	free(v);
	v=NULL;
}
//  --------- CHECK IF NULL --------- //
// This function checking if she recived NULL pointer
int is_null (void* l){
	if (l == NULL)
		return ERROR_NULL;
	return SUCCESS;
}
//  --------- CHECK FSM --------- //
/* This function checks whether FSM has been called  
   Returns 0 on success, -1 on failure*/
int check_fsm(pm_state_t *root){
	if (root->depth != 0 && root->fail == NULL)
		return ERROR;
	else if (root->depth == 0){
		slist_t *list = root->_transitions;
		slist_node_t *node = slist_head(list);
		pm_labeled_edge_t* edgo = (pm_labeled_edge_t*)(node->data);
		pm_state_t* next_state = edgo->state;
		if(next_state->fail == NULL)
			return ERROR;
    	}
	return SUCCESS;
}