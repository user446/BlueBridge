#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <queue.h>

/**
	*	@brief	Инициализация очереди
	*	@note		
	*	@param	q ссылка на тип очереди
	*	@retval	
**/
//
void queue_init(struct Queue* q)
{
//	q->front = (struct Node*) malloc(sizeof(struct Node));
//	q->last = (struct Node*) malloc(sizeof(struct Node));
	q->front = NULL;
	q->last = NULL;
	q->max_len = MAX_QLEN;
//	free(q->front);
//	free(q->last);
	q->size = 0;
}
//

/**
	*	@brief	Очистка очереди
	*	@note		
	*	@param	q ссылка на тип очереди
	*	@retval	
**/
//
void queue_clear(struct Queue* q)
{
	while(q->size--)
		queue_pop(q);
}
//

/**
	*	@brief	Удалить последний записанный элемент
	*	@note		
	*	@param	q ссылка на тип очереди
	*	@retval	
**/
//
void queue_pop(struct Queue* q)
{
	q->size--;
	struct Node* tmp = q->front;
	q->front = q->front->next;
	free(tmp);
}
//

/**
	*	@brief	Проверка на пустоту
	*	@note		
	*	@param	q ссылка на тип очереди
	*	@retval	
**/
//
bool queue_isempty(struct Queue* q)
{
	return q->size == 0;
}
//

/**
	*	@brief	Добавить элемент в очередь
	*	@note		
	*	@param	q ссылка на тип очереди
	*	@param	string строка
	*	@param	len длина строки
	*	@retval	нет
**/
//
void queue_push(struct Queue* q, unsigned char* string, unsigned short len)
{
	q->size++;
	if(q->front == NULL)
	{
		q->front = (struct Node*) malloc(sizeof(struct Node));
		//q->front->string = (unsigned short*)malloc(len);
		//q->front->string = (unsigned char*)malloc(sizeof(unsigned char)*MAX_QLENGTH);
		memset(q->front->string, 0x00, MAX_STRING_LENGTH);
		memcpy(q->front->string, string, len);
		q->front->len = len;
		q->front->next = NULL;
		q->last = q->front;
	}
	else
	{
		q->last->next = (struct Node*) malloc(sizeof(struct Node));
		//q->last->next->string = (unsigned short*)malloc(len);
		//q->last->next->string = (unsigned char*)malloc(sizeof(unsigned char)*len);
		memset(q->last->next->string, 0x00, MAX_STRING_LENGTH);
		memcpy(q->last->next->string, string, len);
		q->last->next->len = len;
		q->last->next->next = NULL;
		q->last = q->last->next;
	}
}
//

/**
	*	@brief	Получить первую записанную строку или ее часть
	*	@note		
	*	@param	q ссылка на тип очереди
	*	@param	string ссылка на строку, в которую производится запись
	*	@param	from начальный элемент с которого происходит запись
	*	@param	to количество записанных элементов
	*	@retval	нет
**/
//
void queue_get_front(struct Queue* q, unsigned char* string, unsigned short from, unsigned short num)
{
	if(from + num <= q->front->len)
		memcpy(string, q->front->string+from, num);
	else
		memcpy(string, q->front->string+from, q->front->len - from);
}
//

/**
	*	@brief	Получить длину первой записанной строки
	*	@note		
	*	@param	q ссылка на тип очереди
	*	@retval	длина первой записанной строки
**/
//
unsigned short queue_get_frontl(struct Queue* q)
{
	return q->front->len;
}
//

/**
	*	@brief	Получить байт из строки по индексу
	*	@note		
	*	@param	q ссылка на тип очереди
	*	@param	index индекс элемента строки, который нужно получить
	*	@retval	длина первой записанной строки
**/
//
unsigned char queue_get_bytefrom(struct Queue* q, unsigned short index)
{
	return q->front->string[index];
}
//

unsigned char* queue_get_first_ptr(struct Queue* q)
{
	return &q->front->string[0];
}
//
