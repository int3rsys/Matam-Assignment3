//
// Created by Omer on 05/12/2017.
//

#ifndef ASSIGNMENT3_NODE_MTM_H
#define ASSIGNMENT3_NODE_MTM_H

typedef void *NodeElement;

typedef int (*compareNodeElement)(NodeElement, NodeElement);
typedef int (*copyNodeElement)(NodeElement, NodeElement);
typedef int (*displayNodeElement)(NodeElement, NodeElement);

typedef struct Node_t *Node;


/*******************************************************************/
/* Interface of data type Node  */


typedef enum {
    NODE_OK, NODE_ELEMENT_OVERIDE
} NodeResult;

//------------------------------------------------------------------------------------------
// Create a new node and connects a given element
// checks by assert element != NULL
// makes duplication for the given element
// returns the new node or NULL if can not create one
Node nodeCreate(NodeElement element, compareNodeElement, displayNodeElement, copyNodeElement);

//------------------------------------------------------------------------------------------
// returns the element that stored by a given node
// element could not be NULL
NodeElement nodeGetElement(Node node);

//------------------------------------------------------------------------------------------
// connects a given "next node" to the next node
// if node's next != NULL returns NODE_ELEMENT_OVERIDE error
// checks by assert node != NULL && next != NULL
// no need for duplication or destroy
// returns NODE_OK, NODE_ELEMENT_OVERIDE
NodeResult nodeAddNext(Node node, Node next);

//------------------------------------------------------------------------------------------
// connects a given "previous node" to the previous node
// if node's next != NULL returns NODE_ELEMENT_OVERIDE error
// checks by assert node != NULL && next != NULL
// no need for duplication or destroy
// returns NODE_OK, NODE_ELEMENT_OVERIDE
NodeResult nodeAddPrevious(Node node, Node previous);

//------------------------------------------------------------------------------------------
// initilize the next node by NULL
// no need for destroy
// returns NODE_OK
NodeResult nodeRemoveNext(Node node);

//------------------------------------------------------------------------------------------
// initilize the previous node by NULL
// no need for destroy
// returns NODE_OK
NodeResult nodeRemovePrevious(Node node);

//------------------------------------------------------------------------------------------
// update an existing element in a given position (index) to be identical
// to a given element. the given element must not be changed.
// no need to duplicate element. just do a simple assignment.
// return values : NODE_OK
NodeResult nodeUpdateElement(Node node, NodeElement element);

//------------------------------------------------------------------------------------------
// display the element in the given index i.
// this function uses displayNodeElement that is guaranteed by the user
// no need to do here '\n'.
// return values : NODE_OK
NodeResult nodeDisplayElement(Node node, displayNodeElement);

//------------------------------------------------------------------------------------------
// deallocate all relevant memory and stop using the node
void nodeDestroy(Node);

//------------------------------------------------------------------------------------------

#endif //ASSIGNMENT3_NODE_MTM_H
