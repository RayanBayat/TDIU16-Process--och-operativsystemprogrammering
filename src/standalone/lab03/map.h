/* do not forget the guard against multiple includes */
#include <stdbool.h>
#include <stdlib.h>

typedef char* value_t;
typedef int key_t;

#define PANIC(message) do { printf("PANIC: %s\n", message); exit(1); } while (0)



struct association 
{
   
    key_t key; /* nyckeln */
    value_t value; /* värdet associerat med nyckeln */
    struct list *prev; /* Previous list element. */
    struct list *next; /* Next list element. */

};


struct map 
{
    struct list *first;
    struct list *last;
    int total;
    
};


   // konstruktor
void map_init(struct map* m);

key_t map_insert(struct map* m, value_t v);

void print(struct map *m);

value_t map_find(struct map* m, key_t k);

value_t map_remove(struct map* m, key_t k);

 void map_for_each(struct map* m, void (*exec)(key_t k, value_t v, int aux), int aux);

 void map_remove_if(struct map* m, bool (*cond)(key_t k, value_t v, int aux), int aux);

 void map_destruct(struct map* m);
