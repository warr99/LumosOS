/*
 * @Author: warrior
 * @Date: 2023-07-19 10:27:57
 * @LastEditors: warrior
 * @LastEditTime: 2023-07-20 11:41:50
 * @Description:
 */
#ifndef LIST_H
#define LIST_H

/**
 * @brief 链表节点
 */
typedef struct _list_node_t {
    struct _list_node_t* pre;
    struct _list_node_t* next;
} list_node_t;

/**
 * @brief 链表
 */
typedef struct _list_t {
    list_node_t* first;
    list_node_t* last;
    int count;
} list_t;

/**
 * @brief 头结点初始化
 * @param {list_node_t*} node
 * @return {*}
 */
static inline void list_node_init(list_node_t* node) {
    node->pre = node->next = (list_node_t*)0;
}

/**
 * @brief 获取结点的前一结点
 * @param {list_node_t*} node 查询的结点
 * @return {list_node_t*} 后继结点
 */
static inline list_node_t* list_node_pre(list_node_t* node) {
    return node->pre;
}

/**
 * @brief 获取结点的后一结点
 * @param {list_node_t*} node 查询的结点
 * @return {list_node_t*} 后继结点
 */
static inline list_node_t* list_node_next(list_node_t* node) {
    return node->next;
}

/**
 * @brief 链表初始化
 * @param {list_t*} list 起始地址
 * @return {*}
 */
void list_init(list_t* list);

/**
 * @brief 判断链表是否为空
 * @param {list_t*} list
 * @return {*} 1 - 空，0 - 非空
 */
static inline int list_is_empty(list_t* list) {
    return list->count == 0;
}

/**
 * @brief 获取链表的
 * @param {list_t*} list 查询的链表
 * @return {*} 结点数量
 */
static inline int list_count(list_t* list) {
    return list->count;
}

/**
 * @brief 获取指定链表的第一个表项
 * @param {list_t*} list 查询的链表
 * @return {*} 第一个表项
 */
static inline list_node_t* list_first(list_t* list) {
    return list->first;
}

/**
 * @brief 获取指定链接的最后一个表项
 * @param {list_t*} list 查询的链表
 * @return {*} 最后一个表项
 */
static inline list_node_t* list_last(list_t* list) {
    return list->last;
}

/**
 * @brief 头插法
 * @param {list_t*} list 链表
 * @param {list_node_t*} node 要插入的节点
 * @return {*}
 */
void list_insert_first(list_t* list, list_node_t* node);

/**
 * @brief 尾插法
 * @param {list_t*} list 链表
 * @param {list_node_t*} node 要插入的节点
 * @return {*}
 */
void list_insert_last(list_t* list, list_node_t* node);

/**
 * @brief 移除指定链表的头部
 * @param {list_t*} list 操作的链表
 * @return {list_node_t*} 链表的第一个结点
 */
list_node_t* list_remove_first(list_t* list);

/**
 * @brief 移除指定链表的中的表项
 * @param {list_t*} list 操作的链表
 * @param {list_node_t*} node 要移除的节点
 * @return {*}
 */
list_node_t* list_remove(list_t* list, list_node_t* node);

#define offset_in_parent(parent_type, node_name) \
    ((uint32_t)(&((parent_type*)0)->node_name))

#define parent_addr(node, parent_type, node_name) \
    ((uint32_t)node - offset_in_parent(parent_type, node_name))

#define list_node_parent(node, parent_type, node_name) \
    ((parent_type*)(node ? parent_addr((node), parent_type, node_name) : 0))

#endif