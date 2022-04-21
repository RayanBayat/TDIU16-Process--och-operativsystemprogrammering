#include <stddef.h>

#include "plist.h"

static struct lock plist_lock;

void plist_init(struct plist* p) 
{
    list_init(&p->content);
    lock_init(&plist_lock);
    p->next_key = 1;
}

pid_t plist_insert(struct plist* p, struct process_information* value)
{
    lock_acquire(&plist_lock);
    struct p_association* new_p_association = (struct p_association*)malloc(sizeof(struct p_association));
    sema_init(&value->pro_sema, 0);
    new_p_association->value = value;
    new_p_association->key = p->next_key++;
    list_push_back(&p->content, &new_p_association->elem);

    lock_release(&plist_lock);
    return new_p_association->key;
}

struct process_information* plist_find(struct plist* p, const pid_t key)
{
    struct list_elem* current;
    struct list_elem* end = list_end(&p->content);

    for(current = list_begin(&p->content); 
        current != end; 
        current = list_next(current)) 
    {
        struct p_association* a = list_entry(current, struct p_association, elem);

        if(key == a->key) 
            return a->value;
    }
    return NULL;
}

struct process_information* plist_remove(struct plist* p, const pid_t key)
{
    lock_acquire(&plist_lock);

    struct list_elem* current;
    struct list_elem* end = list_end(&p->content);

    for(current = list_begin(&p->content); 
        current != end; 
        current = list_next(current)) 
    {
        struct p_association* a = list_entry(current, struct p_association, elem);

        if(key == a->key) 
        {
            struct process_information* value = a->value;
            list_remove(current);
            free(a);
            lock_release(&plist_lock);
            return value;
        }
    }
    lock_release(&plist_lock);
    return NULL;
}

void plist_for_each(struct plist* p, void (*exec)(pid_t, struct process_information*, int aux), int aux)
{
  
    lock_acquire(&plist_lock);
    
    struct list_elem* current;
    struct list_elem* end = list_end(&p->content);

    for(current = list_begin(&p->content); 
        current != end; 
        current = list_next(current)) 
    {
        struct p_association* a = list_entry(current, struct p_association, elem);
        (*exec)(a->key, a->value, aux);
    }
    lock_release(&plist_lock);
}

static void print_value_struct(pid_t key, struct process_information* value, int aux UNUSED)
{
    printf("PROCESS ID: %d\nName: %s\nParent ID: %d\nstatus_code: %d\nparent_alive: %d\nalive: %d\n", 
            key, value->name, value->parent, value->status_code, value->parent_alive, value->alive);
    printf("-----------------------------------------------\n");
}

void plist_printf(struct plist* p)
{
    plist_for_each(p, print_value_struct, 0);
}

void plist_remove_if(struct plist* p, bool (*cond)(pid_t, struct process_information*, int aux), int aux)
{
    lock_acquire(&plist_lock);
    struct list_elem* current = list_begin(&p->content);
    struct list_elem* end = list_end(&p->content);

    while(current != end)
    {
        struct p_association* a = list_entry(current, struct p_association, elem);
        bool to_remove = (*cond)(a->key, a->value, aux);

        if(to_remove)
        {
            current = list_remove(current);
            free(a);
        }
        else
        {
            list_next(current);
        }
    }
    lock_release(&plist_lock);
}


/* function passed as parameter to plist_remove_if in order to free the
 * memory for all inseted values, and return true to remove them from
 * the plist */
static bool do_free(pid_t k UNUSED, struct process_information* v, int aux UNUSED)
{
  free(v);     /*! free memory */
  return true; /*  and remove from collection */
}

// Removes everything that remains in plist
void plist_remove_all(struct plist* p)
{
    plist_remove_if(p, do_free, 0);
}