#include "BubbleSort.h"
void bubbleSort(t_link_element *start,bool (*funcionComparadora)(void*,void*)){
	int swapped;
	t_link_element *ptr1;
	t_link_element *lptr = NULL;

	/* Checking for empty list */
	if (start == NULL)
		return;

	do
	{
		swapped = 0;
		ptr1 = start;

		while (ptr1->next != lptr)
		{
			if(funcionComparadora(ptr1->data,ptr1->next->data))
					{
				swap(ptr1, ptr1->next);
				swapped = 1;
			}
			ptr1 = ptr1->next;
		}
		lptr = ptr1;
	}
	while (swapped);
}

/* function to swap data of two nodes a and b*/
void swap(t_link_element *a, t_link_element *b) {
	void* temp = a->data;
	a->data = b->data;
	b->data = temp;
}


void ordernarLista(t_list* lista, bool (*funcionComparadora)(void*,void*)){
	bubbleSort(lista->head,funcionComparadora);
}
