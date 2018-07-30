#include "slist.h"
#include <stdlib.h>
#define SUCCESS 0
#define ERROR -1
#define ERROR_NULL 1
/* Declarations of functions*/
slist_node_t * create_node(void *data);
void destroy_head (slist_t* l);
int is_nill (void* l);

/* FUNCTIONS */

// This function initializing the list
void slist_init(slist_t * l){
	if (is_nill (l))
		return;
	slist_head(l) = NULL;
	slist_tail(l) = NULL;
	slist_size(l) = 0;
}
// deletes and free the contents of the list
void slist_destroy(slist_t *l, slist_destroy_t flag){
	if (is_nill (l))
		return;
	while ( slist_head(l) != NULL ){
		if(flag == SLIST_FREE_DATA && slist_data(slist_head(l)) != NULL)
			free (slist_data(slist_head(l)));
		destroy_head (l);
	}
	free(slist_head(l));
	slist_head(l)=NULL;
}
// delete the head of the list and return its data
void *slist_pop_first(slist_t *l){
	if (is_nill (l) ||  is_nill (slist_head(l)))
		return NULL;
	void *first = slist_data(slist_head(l));
	destroy_head (l);
	slist_size(l)--;
	if(is_nill (slist_head(l)))
		slist_tail(l)=slist_head(l);
	return first;
}
// add a node to the beginning of the list
int slist_prepend(slist_t * l,void * data){
	if ( is_nill (l) )
		return ERROR; 
	slist_node_t *new_head = create_node(data);
	if (is_nill (new_head))
		return ERROR;
	slist_next(new_head) = slist_head(l);
	slist_head(l) = new_head;
	if (is_nill (slist_head(l)))
		slist_tail(l) = new_head;
	slist_size(l)++;
	return SUCCESS;
}
// add a node to the end of the list
int slist_append(slist_t *l,void *data){
	if (is_nill (l))
		return ERROR;
	slist_node_t *new_tail = create_node(data);
	if (is_nill (new_tail))
		return ERROR;
	slist_next(new_tail) = 	NULL;
	if (is_nill (slist_head(l)))
		slist_head(l) = new_tail;
	else
		slist_next(slist_tail(l)) = new_tail;
	slist_tail(l) = new_tail;
	slist_size(l)++;
	return SUCCESS;
}
// This function connects two lists
int slist_append_list(slist_t* l, slist_t* second){
	if (is_nill (l) || is_nill (second))
		return ERROR;
	slist_node_t *temp = slist_head(second);
	void* data;
	while ( temp != NULL ){
		data = slist_data(temp);
		if( slist_append(l,data) == ERROR )
			return ERROR;
		temp = slist_next(temp);
	}
	return SUCCESS;
}

/* PRIVATE FUNCTIONS */

//  --------- CREATE NEW NODE --------- //
// This function creating a new node
slist_node_t * create_node(void *data){
	slist_node_t *node = (slist_node_t*)malloc(sizeof(slist_node_t));
	if (is_nill (node))
		return NULL;
	slist_data(node) = data;
	return node;
}
//  --------- DESTROY HEAD --------- //
// This function deletes the head of the list
void destroy_head (slist_t* l){
	slist_node_t *temp = slist_head(l);
	slist_head(l) = slist_next(slist_head(l));
	free(temp);
	temp=NULL;
}
//  --------- CHECK IF NULL --------- //
// This function checking if she recived NULL pointer
int is_nill (void* l){
	if (l==NULL)
		return ERROR_NULL;
	return 0;
}
