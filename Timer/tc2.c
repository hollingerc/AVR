/*
 * tc2.c
 *
 * Created: 2016-08-27
 *  Author: Craig Hollinger
 *
 * Functions to access the control registers of Timer/Counter 2.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 3
 * or the GNU Lesser General Public License version 3, both as
 * published by the Free Software Foundation.
 */ 
 
#include <avr/io.h>
#include <timer/tc2.h>

/* tc2_get_config()
 *
 * Fill a pre-defined memory structure with all the Timer/Counter 2 registers.
 * The calling function will modify the structure as required, then call
 * tc0_set_config() to write the data back to the Timer/Counter 2 registers.
 */
void tc2_get_config(TIMER_COUNTER2_TYPE *timer)
{
  timer->tccr2a.reg = TCCR2A;
  timer->tccr2b.reg = TCCR2B;
  timer->tcnt2 = TCNT2;
  timer->ocr2a = OCR2A;
  timer->ocr2b = OCR2B;
  timer->timsk2.reg = TIMSK2;
  timer->tifr2.reg = TIFR2;

}/* end tc2_get_config() */

/* tc2_set_config()
 *
 * Write the contents of the Timer_Counter2 structure back to the Timer/Counter
 * 2 registers.
 */
void tc2_set_config(TIMER_COUNTER2_TYPE *timer)
{
  TCCR2A = timer->tccr2a.reg;
  TCCR2B = timer->tccr2b.reg;
  TCNT2 = timer->tcnt2;
  OCR2A = timer->ocr2a;
  OCR2B = timer->ocr2b;
  TIMSK2 = timer->timsk2.reg;
  TIFR2 = timer->tifr2.reg;

}/* end tc2_set_config() */

/* Write the value to the OCR2A register.
 */
void tc2_set_ocr2a(uint8_t val){
  OCR2A = val;
}

/* Write the value to the OCR2B register.
 */
void tc2_set_ocr2b(uint8_t val){
  OCR2B = val;
}

/* tc2_get_ocr2a()
 *
 * Return the value of the OCR0A register.
 */
uint8_t tc2_get_ocr2a(void)
{
  return(OCR2A);
}

/* tc2_get_ocr2b()
 *
 * Return the value of the OCR0A register.
 */
uint8_t tc2_get_ocr2b(void)
{
  return(OCR2A);
}
