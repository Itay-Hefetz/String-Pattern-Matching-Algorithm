# String-Pattern-Matching-Algorithm

The program allows you to search for a specific set of patterns in a given text, and find their whole appearance
In the text. Its efficiency is O(n), where n is the length of a pattern.


The implementation is done by string matching (automata), using a generic linked list.

how to use it:

* allocate the pm variable
* use the init function to init it.
* add patterns to the automata using the addString function
* complete construction by using the makeFSM function
* make a matches variable and send to it the result of the search function
* Do something with the list of the matches...
* destroy pm with the destroy function
* free the allocated pm pointer

submitted files:

 1. <b>slist.c</b><br>
	This file contains the implementation of the methods from the header file (slist.h),
	So that different actions can be made on the list object, such as initialization, delete, pop, and insert node to the list.

	Additional functions:
	* create_node: this function return a new node that create by malloc, and adding data to this node.
	* destroy_head: this function free the head from the list (after the pointer to the head proceed to next node)
	* is_nill: This function checking if she recived NULL pointer

 2. <b>pattern-matching.c</b><br>
	This file contains the implementation of the methods from the header file,
	So that different actions can be made on the DFA, such as initialization, get/ set state, add string to the DFA, create failure transitions and of course- search in the DFA.
	In this file, the slist.c file is used extensively for the DFA data structures and the various algorithms in the methods
	
	Additional functions:
	* Create State: this function create new state to the DFA, and initializes its values by using the "Init State" function.
	* Init State: this function initializes the values of any new state in DFA, including the root- 'Zero state'.
	* Create Match: this function create new node to the matches list. When a match is found during the run of the "search" function, this method is called and create node.
	* Free DFS: When we call the function "destroy" we will send the root of the DFA to this function. We will perform a "search in depth" in a recursive way, thus enabling us to release all the memory that we gave during running the program without losing information.
	* CHECK IF NULL: This function checking if she recived NULL pointer.
	* Check FSM: This function checks whether FSM has been called.
