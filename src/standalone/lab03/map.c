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
    for (int i = 0; i < m->total; i++)
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
    struct list *curr = m->first;
    struct list *bla;
 
    // if (m->total == 1 && k == 1)
    // {
    //     printf("hej");
    //         free(curr->value);
	// 	free(curr);
    //     return NULL;
    // }
    
    if (k > m->total || k < 1)
    {
        return NULL;
    }
    else{

    
    if (k == 1)
    {
       bla = curr ->next;
       m->first = bla;
        k--;
    }
    else if (k == m->total)
    {
        curr = m->last;
        bla = curr->prev;
        m->last = bla;
    }
    
    else{

    
    
     for (int i = 0; i < k; i++)
     {
        curr = curr -> next;
     }
            bla = curr->prev;
			bla-> next = curr ->next;
			bla = curr ->next;
			bla -> prev = curr -> prev;
    tmp_value = curr->value;
    }
    m->total--;
   // printf("m->total %d/n",m->total);
    for (int i = 0; i <= m->total - k; i++)
    {
        bla ->key--;
        bla = bla ->next;
    }
        
        curr->value= NULL;
		free(curr);
    return tmp_value;
    }
}
void map_for_each(struct map* m, void (*exec)(key_t k, value_t v, int aux), int aux)
{
    struct list *curr = m->last;
    for (int i = 0; i < m->total; i++)
    {
        exec(curr->key,curr->value,aux);
        curr = curr ->prev;
       
    }
    
    
}
void map_remove_if(struct map* m, bool (*cond)(key_t k, value_t v, int aux), int aux)
{
    struct list *curr = m->last;
    struct list *bla;
    while (curr->next != m->first)
    {
        bla = curr->prev;
        if (cond(curr->key,curr->value,aux) == true)
        {
           
           printf("curr key: %d\n", curr->key);
            map_remove(m,curr->key);
            
        }
        curr = bla;
        
        //cond(curr->key,curr->value,aux);
       
       
    }
}






void print(struct map *m){

    struct list *curr = m->last;
    while (curr != m->first)
    {
       printf("%s",curr->value);
       printf(" Key : %d\n",curr->key);

       curr = curr->prev;
       
    }
    
}
void map_destruct(struct map* m)
{
    free(m->first);
    m->last = NULL;
//     m->total = NULL;
//     m->first->value = NULL;
//     m->first->key = NULL;
 }