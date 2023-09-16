#ifndef SNODE_LINKED_LIST_H
#define SNODE_LINKED_LIST_H

/* Standard Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

/* Zephyr Includes */
#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/dlist.h>
#include <zephyr/zephyr.h>

#define COORD_BIT_SIZE 4
#define NODE_NAME_SIZE 12
#define MAC_ADDR_SIZE 20
#define L_NODE_SIZE 12
#define R_NODE_SIZE 12

/* 
    Static Node item
*/
typedef struct static_node_t {
    /* Static Node Device Name*/
    char node_name[NODE_NAME_SIZE];
    char mac_addr[MAC_ADDR_SIZE];

    uint16_t major;
    uint16_t minor;
    uint8_t xPos:COORD_BIT_SIZE;
    uint8_t yPos:COORD_BIT_SIZE;
    
    char l_node[L_NODE_SIZE];
    char r_node[R_NODE_SIZE];
    sys_dnode_t node;

} static_node;

/* Init of gloab DLL */
extern sys_dlist_t node_list;
extern int isAdded;

void init_static_node_linked_list();

void add_static_node(static_node *new_node, char* node_name, char* mac_addr, 
    int major, int minor, int xPos, int yPos, char* l_node, char* r_node);

void add_given_node_at_position(char given_node, int position);

void remove_node(char node_name);

void display_static_node_data(static_node *node);

#endif