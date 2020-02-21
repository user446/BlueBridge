#ifndef __SW_TIMER_H
#define __SW_TIMER_H

#include <stdbool.h>

#define sw_timer_base_100us 1.0f 	//срабатываний таймера каждые 100 микросекунд
#define sw_timer_base_ms 10.0f		//срабатываний таймера каждую милисекунду
#define sw_timer_base_s 10000.0f	//срабатываний таймера каждую секунду

#define sw_timer_100us_insec 10000.0f
#define sw_timer_ms_insec 1000.0f
#define sw_timer_s_insec 1.0f

/**
	*	@brief	структура таймера
	*	@note		
**/
//
struct timer 
{
	volatile bool set;
	volatile unsigned long long start;
	volatile unsigned long long interval;
	void (*callback)(void);
	volatile bool on_interrupt;
	volatile bool autorestart;
};
//

void Timer_set(struct timer* t, float frequency, void (*callback)(void), bool on_interrupt, bool autorestart, bool is_set);
void Timer_reset(struct timer* t);
void Timer_restart(struct timer* t);
void Timer_enable(struct timer* t);
void Timer_disable(struct timer* t);
bool Timer_expired(struct timer* t);
unsigned long long Timer_remaining(struct timer* t);

void t_OnDigitCompleteInterrupt(void);
void t_OnDigitCompleteContinuous(void);

#endif
