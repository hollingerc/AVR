/*
 * File:      itg3205.c
 * Date:      November 2, 2014
 * Author:    Craig Hollinger
 *
 * Driver functions for the InvenSense ITG3205 - 3-axis gyroscope.
 *
 * The register descriptions are taken directly from the InvenSense document:
 * ITG-3205 Product Specification Revision 1.0
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 3
 * or the GNU Lesser General Public License version 3, both as
 * published by the Free Software Foundation.
 */
#include <i2c/i2c.h>

/*
 * Device slave address
 *
 * The ADO pin is pulled low on the breakout board so the address is 0x68.
 * left shift 1 bit: 0xd0
 */
#define ITG3205SlaveAdrs 0x68

/*
 * Register 0 – Who Am I
 *
 * Bit7 | Bit6 Bit5 Bit4 Bit3 Bit2 Bit1 | Bit0
 * -------------------------------------------
 *   0  |               ID              |  -
 *
 * This register is used to verify the identity of the device.
 *
 * ID - Contains the I2C address of the device, which can also be changed by 
 *      writing to this register. The Bit7 should always be set to “0”.
 *      The Power-On-Reset value of Bit6: Bit1 is 110 100.
 */
#define ITG3205_WHOAMI 0x00

uint8_t itg3205_getWhoAmI(void)
{
  uint8_t data;

  i2c_read(ITG3205SlaveAdrs, 1, ITG3205_WHOAMI, &data);
  return(data);

}/* end itg3205_getWhoAmI() */

/*
 * Register 21 – Sample Rate Divider
 *
 * This register determines the sample rate of the ITG-3205 gyros. The gyros' 
 * outputs are sampled internally at either 1kHz or 8kHz, determined by the 
 * DLPF_CFG setting (see register 22). This sampling is then filtered digitally
 * and delivered into the sensor registers after the number of cycles determined 
 * by this register. The sample rate is given by the following formula:
 *
 *  Fsample = Finternal / (divider+1), where Finternal is either 1kHz or 8kHz
 *
 * As an example, if the internal sampling is at 1kHz, then setting this
 * register to 7 would give the following:
 *
 *  Fsample = 1kHz / (7 + 1) = 125Hz, or 8ms per sample
 */
#define ITG3205_SMPLRT_DIV 0x15

void itg3205_setSampleRate(uint8_t rate)
{

  i2c_write(ITG3205SlaveAdrs, 1, ITG3205_SMPLRT_DIV, &rate);

}/* end itg3205_setSampleRate() */

/*
 * Register 22 – DLPF, Full Scale
 *
 * Bit7 Bit6 Bit5 | Bit4 Bit3 | Bit2 Bit1 Bit0
 * -------------------------------------------
 *        -       |   FS_SEL  |    DLPF_CFG
 *
 * This register configures several parameters related to the sensor
 * acquisition.
 *
 * The FS_SEL parameter allows setting the full-scale range of the gyro sensors,
 * as described in the table below. The power-on-reset value of FS_SEL is 00h.
 * Set to 03h for proper operation.
 *
 *  FS_SEL | Gyro Full-Scale Range
 *  ------------------------------
 *    0    |      Reserved
 *    1    |      Reserved
 *    2    |      Reserved
 *    3    |     ±2000°/sec
 *
 * The DLPF_CFG parameter sets the digital low pass filter configuration. It
 * also determines the internal sampling rate used by the device as shown in the
 * table below.
 *
 *  DLPF_CFG | Low Pass Filter Bandwidth | Internal Sample Rate
 *  -----------------------------------------------------------
 *      0    |          256Hz            |         8kHz
 *      1    |          188Hz            |         1kHz
 *      2    |           98Hz            |         1kHz
 *      3    |           42Hz            |         1kHz
 *      4    |           20Hz            |         1kHz
 *      5    |           10Hz            |         1kHz
 *      6    |            5Hz            |         1kHz
 *      7    |         Reserved          |       Reserved
 */
#define ITG3205_DLPF_SCALE 0x16

void itg3205_setDlpfScale(uint8_t data)
{

  i2c_write(ITG3205SlaveAdrs, 1, ITG3205_DLPF_SCALE, &data);

}/* end itg3205_setDlpfScale() */

/*
 * Register 23 – Interrupt Configuration
 *
 *  Bit7 | Bit6 | Bit5  |  Bit4  | Bit3 |  Bit2  | Bit1 |  Bit0
 *  -------------------------------------------------------------
 *  ACTL | OPEN | LATCH | LATCH  |  0   | EI_DEV |  0   | EI_DATA
 *       |      | MODE  | CLEAR  |      |        |      |
 *
 * This register configures the interrupt operation of the device. The interrupt
 * output pin (INT) configuration can be set, the interrupt latching/clearing
 * method can be set, and the triggers for the interrupt can be set.
 *
 * Note that if the application requires reading every sample of data from the
 * ITG-3205 part, it is best to enable the raw data ready interrupt (RAW_RDY_EN).
 * This allows the application to know when new sample data is available.
 *
 * ACTL        Logic level for INT output pin – 1=active low, 0=active high
 * OPEN        Drive type for INT output pin – 1=open drain, 0=push-pull
 * LATCH_MODE  Latch mode – 1=latch until interrupt is cleared, 0=50us pulse
 * LATCH_CLEAR Latch clear method – 1=any register read, 0=status register read only
 * EI_DEV      Enable interrupt when device is ready (PLL ready after changing clock source)
 * EI_DATA     Enable interrupt when data is available
 */
#define ITG3205_INT_CNFG 0x17

void itg3205_setInterruptConfig(uint8_t data)
{

  i2c_write(ITG3205SlaveAdrs, 1, ITG3205_INT_CNFG, &data);

}/* end itg3205_setInterruptConfig() */

/*
 * Register 26 – Interrupt Interrupt Status
 *
 *  Bit7 Bit6 Bit5 Bit4 Bit3 |  Bit2   | Bit1 |     Bit0
 *  ---------------------------------------------------------
 *                           | PLL_RDY |      | RAW_DATA_RDY
 *
 * This register is used to determine the status of the ITG-3205 interrupts.
 * Whenever one of the interrupt sources is triggered, the corresponding bit
 * will be set. The polarity of the interrupt pin (active high/low) and the
 * latch type (pulse or latch) has no affect on these status bits.
 *
 * Use the Interrupt Configuration register (23) to enable the interrupt
 * triggers.  If the interrupt is not enabled, the associated status bit will
 * not get set.
 *
 * In normal use, the RAW_DATA_RDY interrupt is used to determine when new
 * sensor data is available in either the sensor registers (27 to 32).
 *
 * Interrupt Status bits get cleared as determined by LATCH_CLEAR in the
 * interrupt configuration register (23).
 */
#define ITG3205_INT_STAT 0x1a

uint8_t itg3205_getInterruptStatus(void)
{
  uint8_t data;

  i2c_read(ITG3205SlaveAdrs, 1, ITG3205_INT_STAT, &data);
  return(data);

}/* end itg3205_getInterruptStatus() */

/*
 * Registers 27 to 34 – Sensor Registers
 *
 * These registers contain the gyro and temperature sensor data for the ITG-3205
 * parts. At any time, these values can be read from the device; however it is
 * best to use the interrupt function to determine when new data is available.
 *
 * Note: Data is in big endian format.
 */
#define ITG3205_TEMP_OUT 0x1b
#define ITG3205_GYRO_DATA 0x1d
#define ITG3205_GYRO_XOUT 0x1d
#define ITG3205_GYRO_YOUT 0x1f
#define ITG3205_GYRO_ZOUT 0x21

/*
  itg3205_getXGyroData()
  itg3205_getYGyroData()
  itg3205_getZGyroData()

Read the values of the X, Y, and Z gyroscope data.
*/
int16_t itg3205_getXData(void)
{
  int16_t data;
  uint8_t buf[2];
  
  i2c_read(ITG3205SlaveAdrs, 2, ITG3205_GYRO_XOUT, buf);
  data = (int16_t)buf[0]<<8;
  data += (int16_t)buf[1];

  return(data);

}/* end itg3205_getXGyroData() */

int16_t itg3205_getYData(void)
{
  int16_t data;
  uint8_t buf[2];

  i2c_read(ITG3205SlaveAdrs, 2, ITG3205_GYRO_YOUT, buf);
  data = (int16_t)buf[0]<<8;
  data += (int16_t)buf[1];

  return(data);

}/* end itg3205_getYGyroData() */

int16_t itg3205_getZData(void)
{
  int16_t data;
  uint8_t buf[2];

  i2c_read(ITG3205SlaveAdrs, 2, ITG3205_GYRO_ZOUT, buf);
  data = (int16_t)buf[0]<<8;
  data += (int16_t)buf[1];

  return(data);

}/* end itg3205_getZGyroData() */

/*
 * itg3205_getGyroData()
 *
 * Read the values of all the X, Y, and Z gyroscope data.
 */
void itg3205_getGyroData(uint8_t *buf)
{

  i2c_read(ITG3205SlaveAdrs, 6, ITG3205_GYRO_DATA, buf);

}/* end itg3205_getGyroData() */

/*
 * itg3205_getTempData()
 *
 * Read the values of the temperature data.
 */
int16_t itg3205_getTempData(void)
{
  int16_t data;
  uint8_t buf[2];

  i2c_read(ITG3205SlaveAdrs, 2, ITG3205_TEMP_OUT, buf);
  data = (int16_t)buf[0]<<8;
  data += (int16_t)buf[1];

  return(data);

}/* end itg3205_getTempData() */

/*
 * Register 62 – Power Management
 *
 *   Bit7   | Bit6  |  Bit5   |  Bit4   |   Bit3  |  Bit2  | Bit1 |  Bit0
 *  ---------------------------------------------------------------------
 *  H_RESET | SLEEP | STBY_XG | STBY_YG | STBY_ZG |         CLK_SEL
 *
 * This register is used to manage the power control, select the clock source,
 * and to issue a master reset to the device.
 * Setting the SLEEP bit in the register puts the device into very low power
 * sleep mode. In this mode, only the serial interface and internal registers
 * remain active, allowing for a very low standby current. Clearing this bit
 * puts the device back into normal mode. To save power, the individual standby
 * selections for each of the gyros should be used if any gyro axis is not used
 * by the application.
 *
 * The CLK_SEL setting determines the device clock source as follows:
 *
 *  CLK_SEL |           Clock Source
 *  -----------------------------------------------
 *     0    | Internal oscillator
 *     1    | PLL with X Gyro reference
 *     2    | PLL with Y Gyro reference
 *     3    | PLL with Z Gyro reference
 *     4    | PLL with external 32.768kHz reference
 *     5    | PLL with external 19.2MHz reference
 *     6    | Reserved
 *     7    | Reserved
 *
 * On power up, the ITG-3205 defaults to the internal oscillator. It is highly
 * recommended that the device is configured to use one of the gyros (or an
 * external clock) as the clock reference, due to the improved stability.
 *
 * H_RESET Reset device and internal registers to the power-up-default settings
 * SLEEP   Enable low power sleep mode
 * STBY_XG Put gyro X in standby mode (1=standby, 0=normal)
 * STBY_YG Put gyro Y in standby mode (1=standby, 0=normal)
 * STBY_ZG Put gyro Z in standby mode (1=standby, 0=normal)
 * CLK_SEL Select device clock source
 */
#define ITG3205_PWR_MGMT 0x3e

void itg3205_setPowerMgmt(uint8_t data)
{

  i2c_write(ITG3205SlaveAdrs, 1, ITG3205_PWR_MGMT, &data);

}/* end itg3205_setPowerMgmt() */

