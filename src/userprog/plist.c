#include <stddef.h>

#include "plist.h"

static struct lock plist_lock;

void plist_init(struct plist* p) 
{
    list_init(&p->content);
    lock_init(&plist_lock);
    p->next_key = 1;
}
static void print_value_struct(pid_t key, struct process_information* value, int aux UNUSED)
{
    printf("PROCESS ID: %d\nName: %s\nParent ID: %d\nstatus_code: %d\nparent_alive: %d\nalive: %d\n", 
            key, value->name, value->parent, value->status_code, value->parent_alive, value->alive);
    printf("-----------------------------------------------\n");
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
  
     struct list_elem* curr = list_begin(&p->content);
    struct list_elem* tail = list_end(&p->content);

    for(; curr != tail; curr = list_next(curr)) 
    {
        struct p_association* tmp = list_entry(curr, struct p_association, elem);
       if(key == tmp->key) 
            return tmp->value;
    }
    return NULL;
}

struct process_information* plist_remove(struct plist* p, const pid_t key)
{
    lock_acquire(&plist_lock);

    struct list_elem* curr = list_begin(&p->content);
    struct list_elem* tail = list_end(&p->content);

    for(; curr != tail; curr = list_next(curr)) 
    {
        struct p_association* tmp = list_entry(curr, struct p_association, elem);

       if(key == tmp->key) 
        {
        //     print_value_struct(tmp->key,tmp->value,2);
            struct process_information* value = tmp->value;
            list_remove(curr);
            free(tmp);
            lock_release(&plist_lock);
            return value;
        }
    }
   //  printf("removed\n");
    lock_release(&plist_lock);
    return NULL;
}

void plist_for_each(struct plist* p, void (*exec)(pid_t, struct process_information*, int aux), int aux)
{
  
    lock_acquire(&plist_lock);
    
    struct list_elem* curr = list_begin(&p->content);
    struct list_elem* tail = list_end(&p->content);

    for(; curr != tail; curr = list_next(curr)) 
    {
         struct p_association* tmp = list_entry(curr, struct p_association, elem);

        (*exec)(tmp->key, tmp->value, aux);
    }
        lock_release(&plist_lock);
}



void plist_printf(struct plist* p)
{
    plist_for_each(p, print_value_struct, 0);
}

void plist_remove_if(struct plist* p, bool (*cond)(pid_t, struct process_information*, int aux), int aux)
{
    lock_acquire(&plist_lock);
    struct list_elem* curr = list_begin(&p->content);
    struct list_elem* tail = list_end(&p->content);
    while(curr != tail)
    {
    //  for(; curr != tail; curr = list_next(curr)) 
    // {
        // if (a >= process_list_size(p))
        // {
        //     break;
        // }
       
        struct p_association* tmp = list_entry(curr, struct p_association, elem);
       // printf("pos = %d\n",tmp->key);
        //print_value_struct(tmp->key,tmp->value,aux);
        if((*cond)(tmp->key, tmp->value, aux))
        {
           
          //  printf("removed\n");
            curr = list_remove(curr);
            free(tmp);
           
        }
        else
        {
         //   printf("numbers %d\n",curr->next);
            curr = list_next(curr);
            
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
size_t process_list_size(struct plist* p)
{
  return list_size(&p->content);
}
