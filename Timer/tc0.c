/*
 * tc0.c
 *
 * Created: 2015-08-09
 *  Author: Craig Hollinger
 *
 * Functions to access the control registers of Timer/Counter 0.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 3
 * or the GNU Lesser General Public License version 3, both as
 * published by the Free Software Foundation.
 */ 
 
#include <avr/io.h>
#include <timer/tc0.h>

/* tc0_get_config()
 *
 * Fill a pre-defined memory structure with all the Timer/Counter 0 registers.
 * The calling function will modify the structure as required, then call
 * tc0_set_config() to write the data back to the Timer/Counter 0 registers.
 */
void tc0_get_config(Timer_Counter0 timer)
{
  timer.tccr0a.reg = TCCR0A;
  timer.tccr0b.reg = TCCR0B;
  timer.tcnt0 = TCNT0;
  timer.ocr0a = OCR0A;
  timer.ocr0b = OCR0B;
  timer.timsk0.reg = TIMSK0;
  timer.tifr0.reg = TIFR0;

}/* end tc0_get_config() */

/* tc0_set_config()
 *
 * Write the contents of the Timer_Counter0 structure back to the Timer/Counter
 * 0 registers.
 */
void tc0_set_config(Timer_Counter0 timer)
{
  TCCR0A = timer.tccr0a.reg;
  TCCR0B = timer.tccr0b.reg;
  TCNT0 = timer.tcnt0;
  OCR0A = timer.ocr0a;
  OCR0B = timer.ocr0b;
  TIMSK0 = timer.timsk0.reg;
  TIFR0 = timer.tifr0.reg;

}/* end tc0_set_config() */

/* tc0_set_ocr0a()
*
 * Write val to the OCR0A register.
 */
void tc0_set_ocr0a(uint8_t val)
{
  OCR0A = val;
}

/* tc0_set_ocr0b()
 *
 * Write val to the OCR0B register.
 */
void tc0_set_ocr0b(uint8_t val)
{
  OCR0B = val;
}

/* tc0_get_ocr0a()
 *
 * Return the value of the OCR0A register.
 */
uint8_t tc0_get_ocr0a(void)
{
  return(OCR0A);
}

/* tc0_get_ocr0b()
 *
 * Return the value of the OCR0A register.
 */
uint8_t tc0_get_ocr0b(void)
{
  return(OCR0A);
}
