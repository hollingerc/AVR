/*
 * File:      hmc5883.h
 * Date:      August 4, 2014
 * Author:    Craig Hollinger
 *
 * Public interface for Honeywell HMC5883L triple-axis magnetometer.
 *
 * The register descriptions are take from the data sheet for the magnetometer.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 3
 * or the GNU Lesser General Public License version 3, both as
 * published by the Free Software Foundation.
 */
#ifndef _HMC5883_H_
#define _HMC5883_H_ 1

/*********************************************************************************
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
 * MS1:0 - Measurement Configuration Bits. These bits define the measurement flow 
 *         of the device, specifically whether or not to incorporate an applied 
 *         bias into the measurement.
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
#define HMC5883_AVRG_1 0b00000000
#define HMC5883_AVRG_2 0b00100000
#define HMC5883_AVRG_4 0b01000000
#define HMC5883_AVRG_8 0b01100000

#define HMC5883_DORT_0075 0b00000000
#define HMC5883_DORT_0150 0b00000100
#define HMC5883_DORT_0300 0b00001000
#define HMC5883_DORT_0750 0b00001100
#define HMC5883_DORT_1500 0b00010000
#define HMC5883_DORT_3000 0b00010100
#define HMC5883_DORT_7500 0b00011000

#define HMC5883_MESC_NORM 0b00000000
#define HMC5883_MESC_POS  0b00000001
#define HMC5883_MESC_NEG  0b00000010

/*********************************************************************************
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
#define HMC5883_GAIN_073 0b00000000
#define HMC5883_GAIN_092 0b00100000
#define HMC5883_GAIN_122 0b01000000
#define HMC5883_GAIN_152 0b01100000
#define HMC5883_GAIN_227 0b10000000
#define HMC5883_GAIN_256 0b10100000
#define HMC5883_GAIN_303 0b11000000
#define HMC5883_GAIN_435 0b11100000

/*********************************************************************************
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
#define HMC5883_MODE_NS   0b00000000
#define HMC5883_MODE_HS   0b10000000

#define HMC5883_MODE_CONT 0b00000000
#define HMC5883_MODE_SNGL 0b00000001
#define HMC5883_MODE_IDLE 0b00000011

/* function prototypes */
void hmc5883_init(uint8_t cra, uint8_t crb, uint8_t mode);

char hmc5883_getStatus(void);

int16_t hmc5883_getXData(void);
int16_t hmc5883_getYData(void);
int16_t hmc5883_getZData(void);
void hmc5883_getMagData(uint8_t *buf);

#endif /* _HMC5883_H_ */
