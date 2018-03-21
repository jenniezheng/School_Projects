/*
 * SortedList (and SortedListElement)
 *
 *	A doubly linked list, kept sorted by a specified key.
 *	This structure is used for a list head, and each element
 *	of the list begins with this structure.
 *
 *	The list head is in the list, and an empty list contains
 *	only a list head.  The list head is also recognizable because
 *	it has a NULL key pointer.

struct SortedListElement {
	struct SortedListElement *prev;
	struct SortedListElement *next;
	const char *key;
};
typedef struct SortedListElement SortedList_t;
typedef struct SortedListElement SortedListElement_t;


 * variable to enable diagnositc yield calls

extern int opt_yield;
#define	INSERT_YIELD	0x01	// yield in insert critical section
#define	DELETE_YIELD	0x02	// yield in delete critical section
#define	LOOKUP_YIELD	0x04	// yield in lookup/length critical esction
 */
#define _GNU_SOURCE
#include <pthread.h>
#include "SortedList.h"
#include "utilities.h"
#include <string.h>
#include <stdio.h>

extern int DEBUG_FLAG, YIELD_FLAG;

/*
 * SortedList_insert ... insert an element into a sorted list
 *
 *	The specified element will be inserted in to
 *	the specified list, which will be kept sorted
 *	in ascending order based on associated keys
 *
 * @param SortedList_t *list ... header for the list
 * @param SortedListElement_t *element ... element to be added to the list
 */
void SortedList_insert(SortedList_t *list, SortedListElement_t *element){
	//first argument is a dummy node
	if(list==NULL||element == NULL) {
		if(DEBUG_FLAG)fprintf(stderr,"Error: attempted to insert null element or insert to null list\n");
		return;
	}
	SortedList_t *next=list->next;
	while(next!=list && strcmp(element->key, next->key) > 0){
		next=next->next;
	}
	if(YIELD_FLAG & INSERT_YIELD)
		pthread_yield();
	SortedList_t *prev=next->prev;
	next->prev=element;
	element->next=next;
	element->prev=prev;
	prev->next=element;
}

/**
 * SortedList_delete ... remove an element from a sorted list
 *
 *	The specified element will be removed from whatever
 *	list it is currently in.
 *
 *	Before doing the deletion, we check to make sure that
 *	next->prev and prev->next both point to this node
 *
 * @param SortedListElement_t *element ... element to be removed
 *
 * @return 0: element deleted successfully, 1: corrtuped prev/next pointers
 *
 */
int SortedList_delete(SortedListElement_t *element){
	if(element==NULL){
		if(DEBUG_FLAG)fprintf(stderr,"Error: attempted to erase a null element\n");
		return 1;
	}
	SortedList_t* prev=element->prev;
	SortedList_t* next=element->next;
	if(prev->next!=element || next->prev!=element){
		//corrupted
		return 1;
	}
	if(YIELD_FLAG & DELETE_YIELD)
		pthread_yield();
	prev->next=next;
	next->prev=prev;
	element=NULL;
	return 0;
}

/**
 * SortedList_lookup ... search sorted list for a key
 *
 *	The specified list will be searched for an
 *	element with the specified key.
 *
 * @param SortedList_t *list ... header for the list
 * @param const char * key ... the desired key
 *
 * @return pointer to matching element, or NULL if none is found
 */
SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key){
	if(list==NULL){
		if(DEBUG_FLAG)fprintf(stderr,"Error: attempted to find a key in a nonexisting list\n");
		return NULL;
	}
	SortedList_t* next=list->next;
	while(next!=list){
		if(YIELD_FLAG & LOOKUP_YIELD)
			pthread_yield();
		if(strcmp(next->key,key)==0){
			return next;
		}
		next=next->next;
	}
	return NULL;
}

/**
 * SortedList_length ... count elements in a sorted list
 *	While enumeratign list, it checks all prev/next pointers
 *
 * @param SortedList_t *list ... header for the list
 *
 * @return int number of elements in list (excluding head)
 *	   -1 if the list is corrupted
 */
int SortedList_length(SortedList_t *list){
	int count=0;
	if(list==NULL || list->prev->next != list) return -1;
	SortedList_t* next=list->next;
	while(next!=list){
		if(next->prev->next != next) return -1;
		count++;
		if(YIELD_FLAG & LOOKUP_YIELD)
			pthread_yield();
		next=next->next;
	}
	return count;
}

