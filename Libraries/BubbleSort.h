#include <commons/collections/list.h>
#include <commons/collections/node.h>
#include "Libraries.h"

void bubbleSort(t_link_element *start,bool (*funcionComparadora)(void*,void*));
void swap(t_link_element *a, t_link_element *b);
void ordernarLista(t_list* lista, bool (*funcionComparadora)(void*,void*));
