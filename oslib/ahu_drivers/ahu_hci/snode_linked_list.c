/**
***************************************************************
* @file ~/csse4011/repo/ahu_drivers_ahu/snode_linked_list.c
* @author Riley Mosca - 45358195
* @date 20 - 03 - 2023
* @brief Source file which handles the creation and manipulation
         of the doubly linked list for static nodes
***************************************************************
*/
#include "snode_linked_list.h"
#include <zephyr/sys/dlist.h>

sys_dlist_t node_list;
char* l_node_removed;
char* r_node_removed;
int xPos_removed;
int yPos_removed;

void init_static_node_linked_list(void) {

    /* Create a new list, only if the static node list is empty*/
    if(sys_dlist_is_empty(&node_list)) {

        /* Dynamically allocate room for node using malloc */
        static_node* static_node_a = (static_node*)k_malloc(sizeof(static_node));
        static_node* static_node_b = (static_node*)k_malloc(sizeof(static_node));
        static_node* static_node_c = (static_node*)k_malloc(sizeof(static_node));
        static_node* static_node_d = (static_node*)k_malloc(sizeof(static_node));
        static_node* static_node_e = (static_node*)k_malloc(sizeof(static_node));
        static_node* static_node_f = (static_node*)k_malloc(sizeof(static_node));
        static_node* static_node_g = (static_node*)k_malloc(sizeof(static_node));
        static_node* static_node_h = (static_node*)k_malloc(sizeof(static_node));  

        /* Store the data for each node */
        add_static_node(static_node_a, "4011-A", "F5:75:FE:85:34:67", 2753, 32998, 0, 0, "4011-H", "4011-B");
        add_static_node(static_node_b, "4011-B", "E5:73:87:06:1E:86", 32975, 20959, 2, 0, "4011-A", "4011-C");
        add_static_node(static_node_c, "4011-C", "CA:99:9E:FD:98:B1", 26679, 40363, 4, 0, "4011-B", "4011-D");
        add_static_node(static_node_d, "4011-D", "CB:1B:89:82:FF:FE", 41747, 38800, 4, 2, "4011-C", "4011-E");
        add_static_node(static_node_e, "4011-E", "D4:D2:A0:A4:5C:AC", 30679, 51963, 4, 4, "4011-D", "4011-F");
        add_static_node(static_node_f, "4011-F", "C1:13:27:E9:B7:7C", 6195, 18394, 2, 4, "4011-E", "4011-G");
        add_static_node(static_node_g, "4011-G", "F1:04:48:06:39:A0", 30525, 30544, 0, 4, "4011-F", "4011-H");
        add_static_node(static_node_h, "4011-H", "CA:0C:E0:DB:CE:60", 57395, 28931, 0, 2, "4011-G", "4011-I");

        sys_dnode_init(&static_node_a->node); sys_dlist_append(&node_list, &static_node_a->node);
        sys_dnode_init(&static_node_b->node); sys_dlist_append(&node_list, &static_node_b->node);
        sys_dnode_init(&static_node_c->node); sys_dlist_append(&node_list, &static_node_c->node);
        sys_dnode_init(&static_node_d->node); sys_dlist_append(&node_list, &static_node_d->node);
        sys_dnode_init(&static_node_e->node); sys_dlist_append(&node_list, &static_node_e->node);
        sys_dnode_init(&static_node_f->node); sys_dlist_append(&node_list, &static_node_f->node);
        sys_dnode_init(&static_node_g->node); sys_dlist_append(&node_list, &static_node_g->node);
        sys_dnode_init(&static_node_h->node); sys_dlist_append(&node_list, &static_node_h->node);
    }
}

void add_static_node(static_node *new_node, char* node_name, char* mac_addr, 
    int major, int minor, int xPos, int yPos, char* l_node, char* r_node) {
        
    /* Populate Node name with data*/
    strcpy(new_node->node_name, node_name);

    /* Populate the mac address*/
    strcpy(new_node->mac_addr, mac_addr);

    /* Assign Major/ Minor/ Xpos and YPos data*/
    new_node->major = major;
    new_node->minor = minor;
    new_node->xPos = xPos;
    new_node->yPos = yPos;

    /* Populate the data of the left and right nodes*/
    strcpy(new_node->l_node, l_node);
    strcpy(new_node->r_node, r_node);
}

/*
    Given a node name (4011-A), taking A as the name
    remove the reference of the node in the D List 
    and free the memory
*/
void remove_node(char node_name) {

    /* Declaring iterative node, and character position index*/
    int node_name_pos = 5;
    static_node *nodeCheck;

    /* Iterating through all containers, to find the node of interest*/
    SYS_DLIST_FOR_EACH_CONTAINER(&node_list, nodeCheck, node) {

        /* If the node is in the list, remove it! */
        if(nodeCheck->node_name[node_name_pos] == node_name) {

            l_node_removed = nodeCheck->l_node;
            r_node_removed = nodeCheck->r_node;
            xPos_removed = nodeCheck->xPos;
            yPos_removed = nodeCheck->yPos;

            /* Removing the node from the DLL*/
            sys_dlist_remove(&nodeCheck->node);

            /* Free the memory of the node */
            k_free(nodeCheck);
        }
    }
}

void add_given_node_at_position(char given_node, int position) {

    /* Allocating memory for a node to add */
    static_node *node = (static_node*)k_malloc(sizeof(static_node));

    /* Iterative node for Doubly Linked List */
    static_node* nodeCheck;

    /* Positional indexer */
    int posIdx = 0;

    /* If node exists in the list, return, as we cannot add it*/
    SYS_DLIST_FOR_EACH_CONTAINER(&node_list, nodeCheck, node) {
        if(nodeCheck->node_name[5] == given_node) {

            /* Free memory and return */
            k_free(node);
            return;
        }
    }

    /* Create a node, given the input char */
    switch(given_node) {
        case 'a':
        case 'A':
        
            add_static_node(node, "4011-A", "F5:75:FE:85:34:67", 
                2753, 32998, xPos_removed, yPos_removed, l_node_removed, r_node_removed);
            break;

        case 'b':
        case 'B':
            add_static_node(node, "4011-B", "E5:73:87:06:1E:86",    
                32975, 20959, xPos_removed, yPos_removed, l_node_removed, r_node_removed);
            break;

        case 'c':
        case 'C':
            add_static_node(node, "4011-C", "CA:99:9E:FD:98:B1", 
                26679, 40363, xPos_removed, yPos_removed, l_node_removed, r_node_removed);
            break;

        case 'd':
        case 'D':
            add_static_node(node, "4011-D", "CB:1B:89:82:FF:FE", 
                41747, 38800, xPos_removed, yPos_removed, l_node_removed, r_node_removed);
            break;

        case 'e':
        case 'E':
            add_static_node(node, "4011-E", "D4:D2:A0:A4:5C:AC", 
                30679, 51963, xPos_removed, yPos_removed, l_node_removed, r_node_removed);
            break;

        case 'f':
        case 'F':
            add_static_node(node, "4011-F", "C1:13:27:E9:B7:7C", 
                6195, 18394, xPos_removed, yPos_removed, l_node_removed, r_node_removed);
            break;

        case 'g':
        case 'G':
            add_static_node(node, "4011-G", "F1:04:48:06:39:A0", 
                30525, 30544, xPos_removed, yPos_removed, l_node_removed, r_node_removed);
            break;

        case 'h':
        case 'H':
            add_static_node(node, "4011-H", "CA:0C:E0:DB:CE:60", 
                57395, 28931, xPos_removed, yPos_removed, l_node_removed, r_node_removed);
            break;

        case 'i':
        case 'I':
            add_static_node(node, "4011-I", "D4:7F:D4:7C:20:13", 
                30679, 51963, xPos_removed, yPos_removed, l_node_removed, r_node_removed);
            break;

        case 'j':
        case 'J':
            add_static_node(node, "4011-J", "C1:13:27:E9:B7:7C", 
                6195, 18394, xPos_removed, yPos_removed, l_node_removed, r_node_removed);
            break;

        case 'k':
        case 'K':
            add_static_node(node, "4011-K", "F1:04:48:06:39:A0", 
                30525, 30544, xPos_removed, yPos_removed, l_node_removed, r_node_removed);
            break;

        case 'l':
        case 'L':
            add_static_node(node, "4011-L", "CA:0C:E0:DB:CE:60", 
                57395, 28931, xPos_removed, yPos_removed, l_node_removed, r_node_removed);
            break;
    }

    /* if list is empty, just append*/
    if(sys_dlist_is_empty(&node_list)) {
        sys_dlist_append(&node_list, &node->node);
        return;
    }

    /* If we want to reset the head, pre-pend */
    if(position == 0) {
        sys_dlist_prepend(&node_list, &node->node);
        return;
    }
    /* Initialise the node we wish to add */
    sys_dnode_init(&node->node); 

    /* Node check iterator for D list*/
    static_node *nodeCheckTwo;

    /* Finding the previous node we wish to inser at*/
    static_node *prev_node = NULL;
    
    /* Find the position of the node we want to insert at*/
    SYS_DLIST_FOR_EACH_CONTAINER(&node_list, nodeCheck, node) {
        if (posIdx == position) {
            prev_node = nodeCheck;
            break;
        }
        posIdx++;
    }

    /* Previous node found (successor), add the new node before it*/
    if (prev_node) {
        sys_dlist_insert(&prev_node->node, &node->node);
    }
}

/*
    Display all node data, given 
    a pointer to a static node 
*/
void display_static_node_data(static_node *node) {
    printk(
        "BLE Name: %s, "\
        "MAC Addr: %s, "\
        "Major: %d, "\
        "Minor: %d, "\
        "[X, Y]: [%d, %d], "\
        "Left Node: %s, "\
        "Right Node: %s\r\n",
        node->node_name,
        node->mac_addr,
        node->major,
        node->minor,
        node->xPos,
        node->yPos,
        node->l_node,
        node->r_node
    );
}