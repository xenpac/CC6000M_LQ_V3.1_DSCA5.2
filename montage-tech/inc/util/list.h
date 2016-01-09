/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef _LIST_H
#define _LIST_H

/*!
  list head
  */
typedef struct list_head 
{
/*!
  The pointer to the p_next list head
  */
  struct list_head *p_next;
/*!
  The pointer to the previous list head
  */
  struct list_head *p_prev; 
}list_head_t;

#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define INIT_LIST_HEAD(name) \
	list_head_t name = LIST_HEAD_INIT(name)

/*!
  list_entry - Get the struct for this entry.
  *  ptr:	the &struct list_head pointer.
  *  type:	the type of the struct this is embedded in.
  *  member:	the name of the list_struct within the struct.
  */
#define list_entry(ptr, type, member)	\
	((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

/*!
  list_for_each - Iterate over a list
  * @pos:	the &struct list_head to use as a loop counter.
  * @head:	the head for your list.
  */
#define list_for_each(pos, head)			\
	for (pos = (head)->p_next; pos != (head); pos = pos->p_next)

/*!
  list_for_each_safe	-	iterate over a list safe against removal of list entry
  * @pos:	the &struct list_head to use as a loop counter.
  * @n:		another &struct list_head to use as temporary storage
  * @head:	the head for your list.
  */
#define list_for_each_safe(pos, n, head)	\
	for (pos = (head)->p_next, n = pos->p_next; pos != (head); pos = n, n = pos->p_next)

/*!
  * list_for_each_entry_safe - iterate over list of given type safe against removal of list entry
  * @pos:	the type * to use as a loop cursor.
  * @n:		another type * to use as temporary storage
  * @head:	the head for your list.
  * @member:	the name of the list_struct within the struct.
  */
#define list_for_each_entry_safe(pos, n, head, member)			\
	for (pos = list_entry((head)->p_next, typeof(*pos), member),	\
		n = list_entry(pos->member.p_next, typeof(*pos), member);	\
	     &pos->member != (head); 					\
	     pos = n, n = list_entry(n->member.p_next, typeof(*n), member))

/*!
 * list_for_each_entry	-	iterate over list of given type
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
#define list_for_each_entry(pos, head, member)				\
	for (pos = list_entry((head)->p_next, typeof(*pos), member);	\
	     (pos->member.p_next), &pos->member != (head); 	\
	     pos = list_entry(pos->member.p_next, typeof(*pos), member))
	     
static inline void list_init(list_head_t *p_list)
{
	p_list->p_next = p_list;
	p_list->p_prev = p_list;
}

/*!
  * Insert a new entry between two known consecutive entries.
  *
  * This is only for internal list manipulation where we know
  * the p_prev/p_next entries already!
  */
static inline void __list_add(struct list_head *p_new,
			      struct list_head *p_prev,
			      struct list_head *p_next)
{
	p_next->p_prev = p_new;
	p_new->p_next = p_next;
	p_new->p_prev = p_prev;
	p_prev->p_next = p_new;
}

/*!
  * list_add - add a new entry
  * @new: new entry to be added
  * @head: list head to add it after
  *
  * Insert a new entry after the specified head.
  * This is good for implementing stacks.
  */
static inline void list_add(struct list_head *p_new, struct list_head *p_head)
{
	__list_add(p_new, p_head, p_head->p_next);
}


/*!
  * list_add_tail - add a new entry
  * @new: new entry to be added
  * @head: list head to add it before
  *
  * Insert a new entry before the specified head.
  * This is useful for implementing queues.
  */
static inline void list_add_tail(struct list_head *p_new, struct list_head *p_head)
{
	__list_add(p_new, p_head->p_prev, p_head);
}

/*!
  * Delete a list entry by making the p_prev/p_next entries
  * point to each other.
  *
  * This is only for internal list manipulation where we know
  * the p_prev/p_next entries already!
  */
static inline void __list_del(struct list_head * p_prev, struct list_head * p_next)
{
	p_next->p_prev = p_prev;
	p_prev->p_next = p_next;
}

/*!
  * list_del - deletes entry from list.
  * @entry: the element to delete from the list.
  * Note: list_empty() on entry does not return true after this, the entry is
  * in an undefined state.
  */
static inline void list_del(struct list_head *p_entry)
{
	__list_del(p_entry->p_prev, p_entry->p_next);
	p_entry->p_next = p_entry;
	p_entry->p_prev = p_entry;
}

/*!
  ***************************************************************************
  * Description
  *   Deletes entry from list and reinitialize it.
  * Parameters
  *   entry : The element to delete from the list.
  * Returns
  *   None.
  *
  * Return Value List
  *   None.
  * Remarks
  *   None.
  ***************************************************************************
  */
static inline void list_del_init(struct list_head *p_entry)
{
	__list_del(p_entry->p_prev, p_entry->p_next);
	list_init(p_entry);
}

/*!
  ***************************************************************************
  * Description
  *   Tests whether a list is empty.
  * Parameters
  *   entry : The list to test.
  * Returns
  *   List empty or not.
  *
  * Return Value List
  *   0 : List is empty.
  *   Other value : List isn't empty.
  * Remarks
  *   None.
  ***************************************************************************
  */
static inline int list_empty(struct list_head *p_head)
{
	return (p_head->p_next == p_head);
}

/*!
  * list_move - delete from one list and add as another's head
  * @list: the entry to move
  * @head: the head that will precede our entry
  */
static inline void list_move(struct list_head *p_list, struct list_head *p_head)
{
	__list_del(p_list->p_prev, p_list->p_next);
	list_add(p_list, p_head);
}

/*!
  * list_move_tail - delete from one list and add as another's tail
  * @list: the entry to move
  * @head: the head that will follow our entry
  */
static inline void list_move_tail(struct list_head *p_list,
				  struct list_head *p_head)
{
	__list_del(p_list->p_prev, p_list->p_next);
	list_add_tail(p_list, p_head);
}


static inline void __list_splice(struct list_head *p_list,
				 struct list_head *p_prev,
				 struct list_head *p_next)
{
	struct list_head *p_first = p_list->p_next;
	struct list_head *p_last = p_list->p_prev;

	p_first->p_prev = p_prev;
	p_prev->p_next = p_first;

	p_last->p_next = p_next;
	p_next->p_prev = p_last;
}

/*!
  * list_splice - join two lists, this is designed for stacks
  * @list: the new list to add.
  * @head: the place to add it in the first list.
  */
static inline void list_splice(struct list_head *p_list,
				struct list_head *p_head)
{
	if (!(list_empty(p_list)))
		__list_splice(p_list, p_head, p_head->p_next);
}

/*!
  * list_splice_tail - join two lists, each list being a queue
  * @list: the new list to add.
  * @head: the place to add it in the first list.
  */
static inline void list_splice_tail(struct list_head *p_list,
				struct list_head *p_head)
{
	if (!list_empty(p_list))
		__list_splice(p_list, p_head->p_prev, p_head);
}


/*!
  ***************************************************************************
  * Description
  *   replace old entry by new one
  * Parameters
  *   old : the element to be replaced.
  *   new : the new element to insert.
  * Returns
  *   None.
  *
  * Return Value List
  *   None.
  * Remarks
  *   if 'old' was empty, it will be overwritten..
  ***************************************************************************
  */
static inline void list_replace(struct list_head *p_old, struct list_head *p_new_list)
{
	p_new_list->p_next = p_old->p_next;
	p_new_list->p_next->p_prev = p_new_list;
	p_new_list->p_prev = p_old->p_prev;
	p_new_list->p_prev->p_next = p_new_list;
}

static inline void list_replace_init(struct list_head *p_old, struct list_head *p_new_list)
{
	list_replace(p_old, p_new_list);
	list_init(p_old);
}

/*!
  * list_is_singular - tests whether a list has just one entry.
  * @head: the list to test.
  */
static inline int list_is_singular(struct list_head *p_head)
{
	return !list_empty(p_head) && (p_head->p_next == p_head->p_prev);
}
#endif
