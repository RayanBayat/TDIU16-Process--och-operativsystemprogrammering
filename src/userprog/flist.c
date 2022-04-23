
#include "flist.h"
#include "filesys/filesys.h"

static bool do_free(key_t k UNUSED, value_t v, int aux UNUSED)
{
    //ersätt med filsysclose
    filesys_close(v);
 // free(v);     /*! free memory */
  return true; /*  and remove from collection */
}
void map_init(struct map* m) 
{
    list_init(&(m->content));
    m->next_key = 2;
}

key_t map_insert(struct map* m, const value_t v)
{
    struct association* newac = (struct association*)malloc(sizeof(struct association));
    newac->value = v;
    newac->key = m->next_key++;
    list_push_back(&m->content, &newac->elem);

    return newac->key;
}

value_t map_find(struct map* m, const key_t k)
{
    struct list_elem* curr = list_begin(&m->content);
    struct list_elem* tail = list_end(&m->content);

    for(; curr != tail; curr = list_next(curr)) 
    {
        struct association* tmp = list_entry(curr, struct association, elem);

        if(k == tmp->key) 
            return tmp->value;
    }

    return NULL;
}

value_t map_remove(struct map* m, const key_t k)
{
    struct list_elem* curr = list_begin(&m->content);
    struct list_elem* tail = list_end(&m->content);

    for(; curr != tail; curr = list_next(curr)) 
    {
        struct association* tmp = list_entry(curr, struct association, elem);

        if(k == tmp->key) 
        {
            value_t value = tmp->value;
            list_remove(curr);
            free(tmp);

            return value;
        }
    }
    return NULL;
}

void map_for_each(struct map* m, void (*exec)(key_t, value_t, int aux), int aux)
{
    struct list_elem* curr = list_begin(&m->content);
    struct list_elem* tail = list_end(&m->content);

    for(;curr != tail; curr = list_next(curr))  
    {
        struct association* tmp = list_entry(curr, struct association, elem);
        (*exec)(tmp->key, tmp->value, aux);
    }
}

void map_remove_if(struct map* m, bool (*cond)(key_t, value_t, int aux), int aux)
{
    struct list_elem* curr = list_begin(&m->content);
    struct list_elem* tail = list_end(&m->content);


    while(curr != tail)
    {
        struct association* tmp = list_entry(curr, struct association, elem);
        if((*cond)(tmp->key, tmp->value, aux))
        {
            curr = list_remove(curr);
            free(tmp);
        }
        else
        {
           curr = list_next(curr);
        }
    }
}
void map_freeall(struct map* m)
{

    map_remove_if(m,do_free,0);
}
