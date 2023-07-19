/*
 * @Author: warrior
 * @Date: 2023-07-19 11:17:30
 * @LastEditors: warrior
 * @LastEditTime: 2023-07-19 13:42:39
 * @Description:
 */
#include "tools/list.h"

void list_init(list_t* list) {
    list->first = list->last = (list_node_t*)0;
    list->count - 0;
}

void list_insert_first(list_t* list, list_node_t* node) {
    // 设置好待插入结点的前后，前面为空
    node->next = list->first;
    node->pre = (list_node_t*)0;

    // 如果为空，需要同时设置first和last指向自己
    if (list_is_empty(list)) {
        list->last = list->first = node;
    } else {
        // 否则，设置好原本第一个结点的pre
        list->first->pre = node;

        // 调整first指向
        list->first = node;
    }

    list->count++;
}

void list_insert_last(list_t* list, list_node_t* node) {
    // 设置好结点本身
    node->pre = list->last;
    node->next = (list_node_t*)0;

    // 表空，则first/last都指向唯一的node
    if (list_is_empty(list)) {
        list->first = list->last = node;
    } else {
        // 否则，调整last结点的向一指向为node
        list->last->next = node;

        // node变成了新的后继结点
        list->last = node;
    }

    list->count++;
}

list_node_t* list_remove_first(list_t* list) {
    // 表项为空，返回空
    if (list_is_empty(list)) {
        return (list_node_t*)0;
    }

    // 取第一个结点
    list_node_t* remove_node = list->first;

    // 将first往表尾移1个，跳过刚才移过的那个，如果没有后继，则first=0
    list->first = remove_node->next;
    if (list->first == (list_node_t*)0) {
        // node为最后一个结点
        list->last = (list_node_t*)0;
    } else {
        // 非最后一结点，将后继的前驱清0
        remove_node->next->pre = (list_node_t*)0;
    }

    // 调整node自己，置0，因为没有后继结点
    remove_node->next = remove_node->pre = (list_node_t*)0;

    // 同时调整计数值
    list->count--;
    return remove_node;
}

list_node_t* list_remove(list_t* list, list_node_t* remove_node) {
    // 如果是头，头往前移
    if (remove_node == list->first) {
        list->first = remove_node->next;
    }

    // 如果是尾，则尾往回移
    if (remove_node == list->last) {
        list->last = remove_node->pre;
    }

    // 如果有前，则调整前的后继
    if (remove_node->pre) {
        remove_node->pre->next = remove_node->next;
    }

    // 如果有后，则调整后往前的
    if (remove_node->next) {
        remove_node->next->pre = remove_node->pre;
    }

    // 清空node指向
    remove_node->pre = remove_node->next = (list_node_t*)0;
    list->count--;
    return remove_node;
}
