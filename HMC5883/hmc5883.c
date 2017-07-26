/*
 * File:      hmc5883.c
 * Date:      August 4, 2014
 * Author:    Craig Hollinger
 *
 * Driver functions for Honeywell HMC5883L triple-axis magnetometer.
 *
 * The register descriptions are take from the data sheet for the magnetometer.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 3
 * or the GNU Lesser General Public License version 3, both as
 * published by the Free Software Foundation.
 */
#include <i2c/i2c.h>

/*******************************************************************************
 * device slave address = 0x1e
 *
 * shifted 1 bit left: 0x3c
 */
#define HMC5883SlaveAdrs 0x1e

/*******************************************************************************
 * Configuration Register A
 *
 * The configuration register is used to configure the device for setting the data 
 * output rate and measurement configuration. 
 *
 *  D7  | D6  | D5  | D4  | D3  | D2  | D1  | D0
 * CRA7 | MA1 | MA0 | DO2 | DO1 | DO0 | MS1 | MS0
 *
 * CRA7 - reserved, maintain at 0
 * MA1:0 - Select number of samples averaged (1 to 8) per measurement output.
 *         00 = 1(Default); 01 = 2; 10 = 4; 11 = 8
 * DO2:0 - Data Output Rate Bits. These bits set the rate at which data is written 
 *         to all three data output registers (Hz).
 *
 *         000 | 0.75
 *         001 | 1.5
 *         010 | 3
 *         011 | 7.5
 *         100 | 15 (default)
 *         101 | 30
 *         110 | 75
 *         111 | reserved
 *
 * MS1:0 - Measurement Configuration Bits. These bits define the measurement 
 *         flow of the device, specifically whether or not to incorporate an 
 *         applied bias into the measurement.
 *
 *         00 | Normal measurement configuration (Default). In normal measurement 
 *            | configuration the device follows normal measurement flow. The 
 *            | positive and negative pins of the resistive load are left floating 
 *            | and high impedance.
 *         01 | Positive bias configuration for X, Y, and Z axes. In this 
 *            | configuration, a positive current is forced across the resistive 
 *            | load for all three axes.
 *         10 | Negative bias configuration for X, Y and Z axes. In this 
 *            | configuration, a negative current is forced across the resistive 
 *            | load for all three axes.
 *         11 | reserved
 */
#define HMC5883_CRA 0x00

/*******************************************************************************
 * Configuration Register B
 *
 * The configuration register B for setting the device gain.
 *
 * D7  | D6  | D5  | D4 | D3 | D2 | D1 | D0
 * GN2 | GN1 | GN0 | 0  | 0  | 0  | 0  | 0
 *
 * The table below shows nominal gain settings. Use the “Gain” column to convert 
 * counts to Gauss. The "Digital Resolution" column is the theoretical value in 
 * term of milli-Gauss per count (LSb) which is the inverse of the values in the 
 * “Gain” column. The effective resolution of the usable signal also depends on 
 * the noise floor of the system, i.e.:
 *
 * Effective Resolution = Max (Digital Resolution, Noise Floor)
 *
 * Choose a lower gain value (higher GN#) when total field strength causes 
 * overflow in one of the data output registers (saturation). Note that the very 
 * first measurement after a gain change maintains the same gain as the previous 
 * setting. The new gain setting is effective from the second measurement and on.
 *
 *       | Recommended | Gain   |  Digital   |
 * GN2:0 |   Sensor    | (LSb/  | Resolution |         Output Range
 *       | Field Range | Gauss) |  (mG/LSb)  |
 *  000  |  ± 0.88 Ga  |  1370  |    0.73    | 0xF800–0x07FF (-2048–2047)
 *  001  |  ± 1.3 Ga   |  1090  |    0.92    | 0xF800–0x07FF (-2048–2047) default
 *  010  |  ± 1.9 Ga   |   820  |    1.22    | 0xF800–0x07FF (-2048–2047)
 *  011  |  ± 2.5 Ga   |   660  |    1.52    | 0xF800–0x07FF (-2048–2047)
 *  100  |  ± 4.0 Ga   |   440  |    2.27    | 0xF800–0x07FF (-2048–2047)
 *  101  |  ± 4.7 Ga   |   390  |    2.56    | 0xF800–0x07FF (-2048–2047)
 *  110  |  ± 5.6 Ga   |   330  |    3.03    | 0xF800–0x07FF (-2048–2047)
 *  111  |  ± 8.1 Ga   |   230  |    4.35    | 0xF800–0x07FF (-2048–2047)
 */
#define HMC5883_CRB 0x01

/*******************************************************************************
 * Mode Register
 *
 * This register is used to select the operating mode of the device.
 *
 * D7 | D6 | D5 | D4 | D3 | D2 | D1  | D0
 * HS | 0  | 0  | 0  | 0  | 0  | MD1 | MD0
 *
 * HS - set to 1 to enable high speed I2C (3400kHz) operation
 *
 * MD1:0 - 00 Continuous-Measurement Mode. 
 *            In continuous-measurement mode, the device continuously performs 
 *            measurements and places the result in the data register. RDY goes 
 *            high when new data is placed in all three registers. After a 
 *            power-on or a write to the mode or configuration register, the 
 *            first measurement set is available from all three data output 
 *            registers after a period of 2/fDO and subsequent measurements are 
 *            available at a frequency of fDO, where fDO is the frequency of 
 *            data output.
 *       - 01 Single-Measurement Mode (Default).
 *            When single-measurement mode is selected, device performs a single
 *            measurement, sets RDY high and returned to idle mode. Mode register
 *            returns to idle mode bit values.  The measurement remains in the 
 *            data output register and RDY remains high until the data output 
 *            register is read or another measurement is performed.
 *       - 10 Idle Mode.
 *       - 11 Idle Mode.
 *
 */
#define HMC5883_MODE 0x02
#define HMC_MODE_CONT 0b00000000
#define HMC_MODE_SNGL 0b00000001
#define HMC_MODE_IDLE 0b00000011

/*******************************************************************************
 * Data Output Registers
 *
 * The data output registers are two 8-bit registers. These registers store the 
 * measurement result from the channels. The value stored in these two registers 
 * is a 16-bit value in 2’s complement form, whose range is 0xF800 to 0x07FF.  The
 * MSB is stored in the lower address register.
 *
 * Data Output Register Operation
 *
 * When one or more of the output registers are read, new data cannot be placed 
 * in any of the output data registers until all six data output registers are 
 * read. This requirement also impacts DRDY pin and RDY bit in the Status register,
*  which cannot be cleared until new data is placed in all the output registers.
 */
#define HMC5883_DATAX 0x03
#define HMC5883_DATAY 0x05
#define HMC5883_DATAZ 0x07

/*******************************************************************************
 * Status Register
 *
 * The status register is an 8-bit read-only register. This register is used to 
 * indicate device status.
 *
 * D7 | D6 | D5 | D4 | D3 | D2 |  D1  | D0
 * HS | 0  | 0  | 0  | 0  | 0  | Lock | Rdy
 *
 * Lock - Data output register lock. 
 *        This bit is set when:
 *        1. some but not all for of the six data output registers have been read,
 *        2. Mode register has been read.
 *        When this bit is set, the six data output registers are locked and any 
 *        new data will not be placed in these register until one of these 
 *        conditions are met: 
 *        1. all six bytes have been read, 
 *        2. the mode register is changed, 
 *        3. the measurement configuration (CRA) is changed,
 *        4. power is reset.
 *
 * Rdy  - Ready Bit. 
 *        Set when data is written to all six data registers. Cleared when device 
 *        initiates a write to the data output registers and after one or more of 
 *        the data output registers are written to. When RDY bit is clear it shall 
 *        remain cleared for a 250 µs. DRDY pin can be used as an alternative to 
 *        the status register for monitoring the device for measurement data.
 */
#define HMC5883_STATUS 0x09

/*******************************************************************************
 * Identification Registers
 *
 * The identification registers are used to identify the device.  The registers
 * are read-only and the values stored in each are:
 *
 * A - 01001000
 * B - 00110100
 * C - 00110011
 */
#define HMC5883_IDA 0x0a
#define HMC5883_IDB 0x0b
#define HMC5883_IDC 0x0c

/*
 * hmc5883_init()
 *
 * Description:
 *
 * Setup the data output rate and measurement configuration (CRA), gain (CRB), 
 * and operating mode (MODE).
 */
void hmc5883_init(uint8_t cra, uint8_t crb, uint8_t mode)
{

  i2c_write(HMC5883SlaveAdrs, 1, HMC5883_CRA, &cra);
  i2c_write(HMC5883SlaveAdrs, 1, HMC5883_CRB, &crb);
  i2c_write(HMC5883SlaveAdrs, 1, HMC5883_MODE, &mode);

}/* end hmc5883_init() */

/*
 * hmc5883_getStatus()
 *
 * Return the contents of the Status register (2 bits):
 *
 * LOCK - Data output register lock. This bit is set when:
 *        1. some but not all for of the six data output registers have been read,
 *        2. Mode register has been read.
 *
 *        When this bit is set, the six data output registers are locked and any
 *        new data will not be placed in these register until one of these
 *        conditions are met:
 *        1. all six bytes have been read,
 *        2. the mode register is changed,
 *        3. the measurement configuration (CRA) is changed,
 *        4. power is reset.
 *
 * RDY  - Ready Bit. Set when data is written to all six data registers. Cleared
 *        when device initiates a write to the data output registers and after
 *        one or more of the data output registers are written to. When RDY bit
 *        is clear it shall remain cleared for a 250 ?s. DRDY pin can be used as
 *        an alternative to the status register for monitoring the device for
 *        measurement data. 
 */
uint8_t hmc5883_getStatus(void)
{
  uint8_t data;

  i2c_read(HMC5883SlaveAdrs, 1, HMC5883_STATUS, &data);
 
  return(data);

}/* end hmc5883_getStatus() */

/*
 * hmc5883_getXData()
 * hmc5883_getYData()
 * hmc5883_getZData()
 *
 * Return the magnetometer data (2 bytes) from each of the three axes.
 */
int16_t hmc5883_getXData(void)
{
  int16_t data;
  uint8_t buf[2];

  i2c_read(HMC5883SlaveAdrs, 2, HMC5883_DATAX, buf);
  data = (int16_t)buf[0]<<8;
  data += (int16_t)buf[1];

  return(data);

}/* end hmc5883_getXData() */

int16_t hmc5883_getYData(void)
{
  int16_t data;
  uint8_t buf[2];

  i2c_read(HMC5883SlaveAdrs, 2, HMC5883_DATAY, buf);
  data = (int16_t)buf[0]<<8;
  data += (int16_t)buf[1];

  return(data);

}/* end hmc5883_getYData() */

int16_t hmc5883_getZData(void)
{
  int16_t data;
  uint8_t buf[2];

  i2c_read(HMC5883SlaveAdrs, 2, HMC5883_DATAZ, buf);
  data = (int16_t)buf[0]<<8;
  data += (int16_t)buf[1];

  return(data);

}/* end hmc5883_getZData() */

/*
 * hmc5883_getMagData()
 *
 * Write the magnetometer data (6 bytes) of all three axes into the buffer
 * pointed to by buf.
 *
 * Note: ensure the buffer is large enough to hold 6 bytes of data.
 */
void hmc5883_getMagData(uint8_t buf[])
{

  i2c_read(HMC5883SlaveAdrs, 6, HMC5883_DATAX, buf);

}/* end hmc5883_getMagData() */

