#include "map.h"
#include <stdio.h>

void map_init(struct map *m)
{
    m->first = (struct list *) malloc(sizeof(struct list));
    m->last = m->first;
    m->total = 0;
    m->first->value = '\0';
    m->first->key = 0;
}

key_t map_insert(struct map* m, value_t v)
{
    struct list *curr;

    curr = (struct list *) malloc(sizeof(struct list));

    m->last->next = curr;
    curr->prev = m->last;
    m->last = curr;

    m->last->value = v;

    m->total++;

    m->last->key = m->total;

    m->last->next = NULL;

    return m->last->key;

    
}

value_t map_find(struct map* m, key_t k)
{
    struct list *curr = m->last;


if (k <= m->total && k > 0)
{
    for (size_t i = 0; i < m->total; i++)
    {
        if ( curr->key == k)
        {
            return curr->value;
        }
        
    
       curr = curr->prev;
    }
    
}

    
    return '\0';



}

value_t map_remove(struct map* m, key_t k)
{
    value_t tmp_value;
     struct list *curr = m->last;
     for (int i = 0; i < k; i++)
     {
        curr = curr ->prev;
     }
     struct list *bla = curr->prev;
			bla-> next = curr ->next;
			bla = curr ->next;
			bla -> prev = curr -> prev;
    tmp_value = curr->value;
		free(curr);

     
}

void print(struct map *m){

    struct list *curr = m->last;
    for (size_t i = 0; i < m->total; i++)
    {
       printf("%s",curr->value);
       printf(" Key : %d\n",curr->key);

       curr = curr->prev;
    }
    
}
