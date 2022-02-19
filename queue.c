#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (!head)
        return NULL;

    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;
    // iterate over the list entries and remove it
    element_t *entry, *safe;
    list_for_each_entry_safe (entry, safe, l, list)
        q_release_element(entry);
    free(l);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    // allocate memory for element_t
    element_t *new_entry = malloc(sizeof(element_t));
    if (!new_entry)
        return false;

    size_t len = strlen(s) + 1;
    // allocate memory for 'value' in element_t
    new_entry->value = malloc(len);
    if (!(new_entry->value)) {
        // q_release_element(new_entry);
        free(new_entry);
        return false;
    }
    memcpy(new_entry->value, s, len);
    list_add(&new_entry->list, head);
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head) {
        return false;
    }

    // allocate memory for element_t
    element_t *new_entry = malloc(sizeof(element_t));
    if (!new_entry)
        return false;

    size_t len = strlen(s) + 1;
    // allocate memory for 'value' in element_t
    new_entry->value = malloc(len);
    if (!(new_entry->value)) {
        // q_release_element(new_entry);
        free(new_entry);
        return false;
    }
    memcpy(new_entry->value, s, len);
    list_add_tail(&new_entry->list, head);
    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return target element.
 * Return NULL if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 *
 * NOTE: "remove" is different from "delete"
 * The space used by the list element and the string should not be freed.
 * The only thing "remove" need to do is unlink it.
 *
 * REF:
 * https://english.stackexchange.com/questions/52508/difference-between-delete-and-remove
 */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    if (sp) {
        element_t *entry = list_first_entry(head, element_t, list);
        size_t len = strlen(entry->value);
        len = (bufsize - 1) > len ? len : (bufsize - 1);
        memcpy(sp, entry->value, len);
        sp[len] = '\0';

        list_del(&entry->list);
        return entry;
    }

    return NULL;
}

/*
 * Attempt to remove element from tail of queue.
 * Other attribute is as same as q_remove_head.
 */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    if (sp) {
        element_t *entry = list_last_entry(head, element_t, list);
        size_t len = strlen(entry->value);
        len = (bufsize - 1) > len ? len : (bufsize - 1);
        memcpy(sp, entry->value, len);
        sp[len] = '\0';

        list_del(&entry->list);
        return entry;
    }

    return NULL;
}

/*
 * WARN: This is for external usage, don't modify it
 * Attempt to release element.
 */
void q_release_element(element_t *e)
{
    free(e->value);
    free(e);
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/*
 * Delete the middle node in list.
 * The middle node of a linked list of size n is the
 * ⌊n / 2⌋th node from the start using 0-based indexing.
 * If there're six element, the third member should be return.
 * Return true if successful.
 * Return false if list is NULL or empty.
 */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return NULL;

    struct list_head *slow;
    struct list_head *fast = head->next;

    list_for_each (slow, head) {
        // find the middle node
        if (fast == head || fast->next == head)
            break;
        fast = fast->next->next;
    }
    list_del(slow);

    element_t *mid = list_entry(slow, element_t, list);
    q_release_element(mid);

    return true;
}

/*
 * Delete all nodes that have duplicate string,
 * leaving only distinct strings from the original list.
 * Return true if successful.
 * Return false if list is NULL.
 *
 * Note: this function always be called after sorting, in other words,
 * list is guaranteed to be sorted in ascending order.
 */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head)
        return false;
    element_t *node, *safe;
    char *prev_value = "";
    list_for_each_entry_safe (node, safe, head, list) {
        if (strcmp(prev_value, node->value) == 0) {
            list_del(&node->list);
            q_release_element(node);
        } else {
            prev_value = node->value;
        }
    }
    return true;
}

/*
 * Attempt to swap every two adjacent nodes.
 */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head))
        return;

    struct list_head *node = head;
    while (node->next != head && node->next->next != head) {
        struct list_head *tmp = node->next->next;
        list_del(tmp);
        list_add(tmp, node);
        node = node->next->next;
    }
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return;
    }
    struct list_head *curr = head->next;
    struct list_head *temp = NULL;
    while (curr != head) {
        temp = curr->next;
        curr->next = curr->prev;
        curr->prev = temp;
        curr = temp;
    }
    temp = head->next;
    head->next = head->prev;
    head->prev = temp;
}

struct list_head *merge(struct list_head *l1, struct list_head *l2)
{
    struct list_head *head = NULL;
    struct list_head **indirect = &head;

    while (l1 && l2) {
        element_t *q1 = list_entry(l1, element_t, list);
        element_t *q2 = list_entry(l2, element_t, list);

        if (strcmp(q1->value, q2->value) > 0) {
            *indirect = l2;
            l2 = l2->next;
        } else {
            *indirect = l1;
            l1 = l1->next;
        }
        indirect = &((*indirect)->next);
    }

    if (l1)
        *indirect = l1;
    if (l2)
        *indirect = l2;

    return head;
}

// Doubly-linked list (not circular)
struct list_head *merge_sort_list(struct list_head *head)
{
    if (!head || !head->next)
        return head;

    struct list_head *fast = head->next;
    struct list_head *slow = head;

    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }

    fast = slow->next;
    slow->next = NULL;

    struct list_head *l1 = merge_sort_list(head);
    struct list_head *l2 = merge_sort_list(fast);

    return merge(l1, l2);
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    head->prev->next = NULL;
    head->next = merge_sort_list(head->next);

    struct list_head *prev = head;
    struct list_head *tmp = prev->next;

    while (tmp) {
        tmp->prev = prev;
        tmp = tmp->next;
        prev = prev->next;
    }
    prev->next = head;
    head->prev = prev;
}