#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Node {
        int val;
        struct Node *next;
};

typedef struct Node Node;

Node node9 = {701, NULL};
Node node8 = {109, &node9};
Node node7 = {269, &node8};
Node node6 = {598, &node7};
Node node5 = {437, &node6};
Node node4 = {703, &node5};
Node node3 = {894, &node4};
Node node2 = {508, &node3};
Node node1 = {866, &node2};

char *key = "isrveawhobpnutfg";

void phase_5(char *input);
void phase_6(char *input);
Node *fun6(Node *head);
void explode_bomb();

int main() 
{
        phase_5("m`meka");
        phase_6("701");
}

/********** phase_5 ********
* Purpose:
*      Perform bitwise operations on a 6-char string and use the value of each 	
*  		operation to use as an index against a hardcoded reference string.
* Inputs:
*      char *input: The input string that is used for the bitwise operations.
* Return/Effects:
*      explode_bomb / defuse_bomb
************************/
void phase_5(char* input) 
{
        /* Makes sure that we enter a 6 character string otherwise explode */
        if (strlen(input) != 6) {
                explode_bomb();
        }
        
        char new[7];
        for (int i = 0; i < 6; i++) {
                /* Grab a single byte from the input string located at index i
                        i.e. "movzbl (%rbx, %rax, 1), %edx"                  */
                int32_t c = input[i]; 
                /* Clear all but the 4 lowest bits of the character 
                        i.e. "and $0xf, %edx                        */
                c &= 0xf;
                /* Update the input strings ith character with a character 
                        corresponding to the ith character of key string
                        i.e. "movzbl 0x401dd0 (%rdx), %edx
                              mov %dl, 0x9(%rsp, %rax, 1)"
                */
                new[i] = key[c];
        }
        
        /* Compare the updated input string to the word "titans" and explode
                                                bomb if they are not equal.
        i.e.    "callq 4014fa <strings_not_equal>
                 test %eax, %eax
                 jne 401337 <phase_5+0x58>""
        */
        if (strcmp(new, "titans") != 0) {
                explode_bomb();
        } 
}

/********** phase_6 ********
* Purpose:
*      Sort a linked list in descending and makes sure our input is equal to 4th 
*		index of the sorted list
* Inputs:
*      char *input: The input string
* Return/Effects:
*      explode_bomb / defuse_bomb
************************/
void phase_6(char *input)
{
        /* Extract the number from the input string using strtol method.
        The long num will store the number value that is stored in the
        input string. */
        char *tmp;
        long num = strtol(input, &tmp, 10);

        /* Store the linked list in a pointer variable called head 
        i.e. "mov $0x604320, %edi"                                         */
        Node *head = &node1;
        /* Store the head of the sorted linked list                        */
        Node *newHead = fun6(&node1);
        /* Compare our input number to the 4th node's value of the sorted 
                                                        linked list        */
        if (num != newHead->next->next->next->val) {
                explode_bomb();
        }
}

/********** fun6 ********
* Purpose:
*      Sort a linked list in descending order
* Inputs:
*      Node *head: The head of a linked list
* Return:
*      Node *head: The head of the sorted linked list
************************/
Node *fun6(Node *head) {
        /* Do not sort if the list is 1 element or less  */
        if (head == NULL || head->next == NULL) {
                return head;
        }

        /* Store the unsorted portion of the linked list in current pointer */
        Node *curr = head->next;
        /* Store the sorted portion of the linked list in sorted pointer */
        Node *sorted = head;
        sorted->next = NULL;
        
        while (curr != NULL) {
                /* Store the next node in a next_node variable */
                Node *next_node = curr->next;
                /* If the current value is greater than the head of sorted */
                if (sorted == NULL || sorted->val < curr->val) {
                        /* Update the head of the sorted list */
                        curr->next = sorted;
                        sorted = curr;
                } else { /* The current value is not greater than the head */
                        /* Store the head of sorted list in temp variable */
                        Node *tmp = sorted;
                        /* Loop through the sorted list and find correct 
                                location to insert the current node into */
                        while (tmp->next != NULL && tmp->next->val > curr->val) {
                                tmp = tmp->next;
                        }
                        /* Insert the current node into the sorted list  */
                        curr->next = tmp->next;
                        tmp->next = curr;
                }
                /* Update the current node to the next node */
                curr = next_node;
        }
        return sorted;
}

void explode_bomb()
{
        printf("boom!\n");
}