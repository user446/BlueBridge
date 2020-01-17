#ifndef __QUEUE_H
#define __QUEUE_H

#include <stdbool.h>

#define MAX_QLENGTH 128

/**
	*	@brief	��� �������� ������� �����
	*	@note		
**/
//
typedef struct Node
{
	struct Node* next; 				/// ������ �� ��������� �������
	unsigned short len;				/// ����� ������ � ��������
	unsigned char* string;		/// ������ � ��������
}Node;

/**
	*	@brief	��� ������� �����
	*	@note		
**/
//
typedef struct Queue
{
	struct Node* front; 	/// ������ �� ��������� �������
	struct Node* last;		/// ������ �� ������ ���������� �������
	unsigned int size;		/// ������ �������
}Queue;

void queue_init(struct Queue* q);
void queue_clear(struct Queue* q);
void queue_pop(struct Queue* q);
bool queue_isempty(struct Queue* q);
void queue_push(struct Queue* q, unsigned char* string, unsigned short len);

void queue_get_front(struct Queue* q, unsigned char* string, unsigned short from, unsigned short to);
unsigned short queue_get_frontl(struct Queue* q);
unsigned short queue_get_bytefrom(struct Queue* q, unsigned short index);

#endif
