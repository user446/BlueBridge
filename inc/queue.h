#ifndef __QUEUE_H
#define __QUEUE_H

#include <stdbool.h>

#define MAX_LENGTH 64

/**
	*	@brief	тип элемента очереди строк
	*	@note		
**/
//
typedef struct Node
{
	struct Node* next; 					/// ссылка на следующий элемент
	unsigned short len;									/// длина строки в элементе
	unsigned short string[MAX_LENGTH];		/// строка в элементе
}Node;

/**
	*	@brief	тип очереди строк
	*	@note		
**/
//
typedef struct Queue
{
	struct Node* front; 	/// ссылка на следующий элемент
	struct Node* last;		/// ссылка на первый записанный элемент
	unsigned int size;		/// размер очереди
}Queue;

void queue_init(struct Queue* q);
void queue_clear(struct Queue* q);
void queue_pop(struct Queue* q);
bool queue_isempty(struct Queue* q);
void queue_push(struct Queue* q, unsigned short* string, unsigned short len);

void queue_get_front(struct Queue* q, unsigned short* string, unsigned short from, unsigned short to);
unsigned short queue_get_frontl(struct Queue* q);
unsigned short queue_get_bytefrom(struct Queue* q, unsigned short index);

#endif
