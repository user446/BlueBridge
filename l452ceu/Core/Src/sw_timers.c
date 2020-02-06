#include "sw_timers.h"

static volatile unsigned long long sys_tick_count; ///счетчик срабатываний материнского таймера

static volatile unsigned char t_counter = 0;	///общий счетчик таймеров
static struct timer* timer_stack[128];				///общий массив таймеров

/**
	*	@brief	Проверка таймера на окончание счетчика, сброс и выполнение колбека
	*	@note		
	*	@param	t ссылка на таймер
	*	@retval	нет
**/
//
static void CheckTimer(struct timer* t) 
{
	if(Timer_expired(t) == true) 
	{ 
		if(t->autorestart)
			Timer_restart(t);
		if(t->callback != 0x00)
			t->callback();
	}
}
//


/**
	*	@brief	Проверка всех существующих таймеров на истечение периода работы
	*	@note		Необходимо вставить в callback по окончанию прерывания родительского таймера
	*	@note		Обновляет счетчик срабатываний sys_tick_count
	*	@note		Без включения функции в callback программные таймеры не будут обновляться
	*	@note		Проверка производится только для таймеров имеющих атрибут in_interrupt = true
	*	@param	нет
	*	@retval	нет
**/
//
volatile unsigned char timer_counter_int = 0;
void t_OnDigitCompleteInterrupt(void)
{
	sys_tick_count++;
	timer_counter_int = 0;
 	while(timer_counter_int < t_counter)
	{
		if(timer_stack[timer_counter_int]->on_interrupt == true && timer_stack[timer_counter_int]->set == true)
			CheckTimer(timer_stack[timer_counter_int]);
		timer_counter_int++;
	}
}
//

/**
	*	@brief	Проверка всех существующих таймеров на истечение периода работы
	*	@note		Необходимо вставить в основной цикл программы
	*	@note		Проверка производится только для таймеров имеющих атрибут in_interrupt = false
	*	@param	нет
	*	@retval	нет
**/
//
volatile unsigned char timer_counter_cont = 0;
void t_OnDigitCompleteContinuous(void)
{
	timer_counter_cont = 0;
 	while(timer_counter_cont < t_counter)
	{
		if(timer_stack[timer_counter_cont]->on_interrupt == false && timer_stack[timer_counter_int]->set == true)
			CheckTimer(timer_stack[timer_counter_cont]);
		timer_counter_cont++;
	}
}
//

/**
	*	@brief	Установка таймера
	*	@note		
	*	@param	*t указатель на структуру таймера
	*	@param	frequency частота срабатываний таймера в секунду
	*	@param	time_base позволяет выбрать базу для отсчета, в секундах или в милисекундах
	*	@param	callback указатель на функцию, которую следует исполнить по окончанию периода таймера
	* @param	on_interrupt вызывать callback таймера только по прерывани
	* @param	autorestart включение автоматической перезагрузки таймера
	* @param	is_set включение таймера в проверку
	*	@retval	нет
**/
//
void Timer_set(struct timer* t, float frequency, void (*callback)(void), bool on_interrupt, bool autorestart, bool is_set)
{
	t->set = is_set;
	t->interval = (unsigned long long)(sw_timer_100us_insec/frequency * sw_timer_base_100us);
	t->start = sys_tick_count;
	t->callback = callback;
	timer_stack[t_counter] = t;
	t->on_interrupt = on_interrupt;
	t->autorestart = autorestart;
	t_counter++;
}
//

/**
	*	@brief	Сброс таймера
	*	@note		
	*	@param	*t указатель на структуру таймера
	*	@retval	нет
**/
//
void Timer_reset(struct timer* t)
{
	t->start += t->interval;
}
//

/**
	*	@brief	Рестарт таймера
	*	@note		
	*	@param	*t указатель на структуру таймера
	*	@retval	нет
**/
//
void Timer_restart(struct timer* t)
{
	t->start = sys_tick_count;
}
//

/**
	*	@brief	Проверка на окончание периода работы
	*	@note		
	*	@param	*t указатель на структуру таймера
	*	@retval	истек ли интервал периода таймера
**/
//
bool Timer_expired(struct timer* t)
{
	volatile unsigned long long diff = (sys_tick_count - t->start) + 1;
	return t->interval < diff;
}
//

/**
	*	@brief	Получить количество отсавшихся тиков таймера
	*	@note		
	*	@param	*t указатель на структуру таймера
	*	@retval	оставшееся количество тиков таймера до перезагрузки
**/
//
unsigned long long Timer_remaining(struct timer* t)
{
	return t->start + t->interval - sys_tick_count;
}
//

/**
	*	@brief	Включить таймер в проверку
	*	@note		Перезапускает таймер
	*	@param	*t указатель на структуру таймера
	*	@retval	нет
**/
//
void Timer_enable(struct timer* t)
{
	t->set = true;
	Timer_restart(t);
}
//

/**
	*	@brief	Исключить таймер из проверки
	*	@note		
	*	@param	*t указатель на структуру таймера
	*	@retval	нет
**/
//
void Timer_disable(struct timer* t)
{
	t->set = false;
}
//

/**
	*	@brief	Изменить чстоту срабатывания таймера
	*	@note		
	*	@param	*t указатель на структуру таймера
	*	@param	freq частота
	*	@retval	нет
**/
//
void Timer_frequency(struct timer* t, float frequency)
{
	t->interval = (unsigned long long)(sw_timer_100us_insec/frequency * sw_timer_base_100us);
}
//
