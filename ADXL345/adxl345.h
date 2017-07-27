/*
 * File:      adxl345.h
 * Date:      April 27, 2014
 * Author:    Craig Hollinger
 *
 * Driver functions for Analog Devices ADXL345 triple-axis accelerometer.
 *
 * The ADXL345 register descriptions below are taken from the data sheet.
 */
#ifndef _ADXL345_H_
#define _ADXL345_H_ 1

/*******************************************************************************
 * 0x27 - Activity/Inactivity Control
 *
 *      D7     |       D6       |       D5       |      D4
 *  ACT ac/dc  |  ACT_X enable  |  ACT_Y enable  | ACT_Z enable
 *      D3     |       D2       |       D1       |      D0
 * INACT ac/dc | INACT_X enable | INACT_Y enable | INACT_Z enable
 *
 * ACT AC/DC and INACT AC/DC Bits
 *
 * A setting of 0 selects dc-coupled operation, and a setting of 1 enables ac-
 * coupled operation. In dc-coupled operation, the current acceleration 
 * magnitude is compared directly with THRESH_ACT and THRESH_INACT to determine 
 * whether activity or inactivity is detected.
 *
 * In ac-coupled operation for activity detection, the acceleration value at the
 * start of activity detection is taken as a reference value. New samples of 
 * acceleration are then compared to this reference value, and if the magnitude 
 * of the difference exceeds the THRESH_ACT value, the device triggers an 
 * activity interrupt.
 *
 * Similarly, in ac-coupled operation for inactivity detection, a reference 
 * value is used for comparison and is updated whenever the device exceeds the 
 * inactivity threshold. After the reference value is selected, the device 
 * compares the magnitude of the difference between the reference value and the 
 * current acceleration with THRESH_INACT. If the difference is less than the 
 * value in THRESH_INACT for the time in TIME_INACT, the device is considered 
 * inactive and the inactivity interrupt is triggered.
 *
 * ACT_x Enable Bits and INACT_x Enable Bits
 *
 * A setting of 1 enables x-, y-, or z-axis participation in detecting activity 
 * or inactivity. A setting of 0 excludes the selected axis from participation. 
 * If all axes are excluded, the function is disabled.
 */
#define ADXL_ACT_INACT_CTL_ACT_DC   0b00000000
#define ADXL_ACT_INACT_CTL_ACT_AC   0b10000000
#define ADXL_ACT_INACT_CTL_INACT_DC 0b00000000
#define ADXL_ACT_INACT_CTL_INACT_AC 0b00001000

#define ADXL_ACT_INACT_CTL_ACT_X    0b01000000
#define ADXL_ACT_INACT_CTL_ACT_Y    0b00100000
#define ADXL_ACT_INACT_CTL_ACT_Z    0b00010000
#define ADXL_ACT_INACT_CTL_INACT_X  0b00000100
#define ADXL_ACT_INACT_CTL_INACT_Y  0b00000010
#define ADXL_ACT_INACT_CTL_INACT_Z  0b00000001

/*******************************************************************************
 * 0x2a - Tap Access Enable Bits
 *
 * D7 | D6 | D5 | D4 |     D3   |      D2      |      D1      |      D0
 * 0  | 0  | 0  | 0  | Suppress | TAP_X enable | TAP_Y enable | TAP_Z enable
 * 
  * Suppress Bit
 *
 * Setting the suppress bit suppresses double tap detection if acceleration 
 * greater than the value in THRESH_TAP is present between taps. See the Tap 
 * Detection section for more details.
 *
 * TAP_x Enable Bits
 *
 * A setting of 1 in the TAP_X enable, TAP_Y enable, or TAP_Z enable bit enables
 * x-, y-, or z-axis participation in tap detection. A setting of 0 excludes the
 * selected axis from participation in tap detection.
*/
#define ADXL_TAP_AXES_SUPPRESS_ON   0b00001000
#define ADXL_TAP_AXES_SUPPRESS_OFF  0b00000000
#define ADXL_TAP_AXES_XEN_ON        0b00000100
#define ADXL_TAP_AXES_XEN_OFF       0b00000000
#define ADXL_TAP_AXES_YEN_ON        0b00000010
#define ADXL_TAP_AXES_YEN_OFF       0b00000000
#define ADXL_TAP_AXES_ZEN_ON        0b00000001
#define ADXL_TAP_AXES_ZEN_OFF       0b00000000

/*******************************************************************************
 * 0x2b - Activity Tap Status
 *
 * D7 |      D6      |      D5      |      D4      |
 * 0  | ACT_X source | ACT_Y source | ACT_Z source |
 *  
 *   D3   |      D2      |      D1      |      D0
 * Asleep | TAP_X source | TAP_Y source | TAP_Z source
 * 
 * ACT_x Source and TAP_x Source Bits
 *
 * These bits indicate the first axis involved in a tap or activity event. A 
 * setting of 1 corresponds to involvement in the event, and a setting of 0 
 * corresponds to no involvement. When new data is available, these bits are not
 * cleared but are overwritten by the new data. The ACT_TAP_STATUS register 
 * should be read before clearing the interrupt. Disabling an axis from 
 * participation clears the corresponding source bit when the next activity or 
 * tap/double tap event occurs.
 *
 * Asleep Bit
 *
 * A setting of 1 in the asleep bit indicates that the part is asleep, and a 
 * setting of 0 indicates that the part is not asleep. See the Register 
 * 0x2D—POWER_CTL (Read/Write) section for more information on autosleep mode.
 */
#define ADXL_ACT_TAP_STATUS_ACT_X 0b01000000
#define ADXL_ACT_TAP_STATUS_ACT_Y 0b00100000
#define ADXL_ACT_TAP_STATUS_ACT_Z 0b00010000
#define ADXL_ACT_TAP_STATUS_ASLP  0b00001000
#define ADXL_ACT_TAP_STATUS_TAP_X 0b00000100
#define ADXL_ACT_TAP_STATUS_TAP_Y 0b00000010
#define ADXL_ACT_TAP_STATUS_TAP_Z 0b00000001

/*******************************************************************************
 * 0x2c - Data Rate and Power Control
 *
 * D7 | D6 | D5 |     D4    | D3 | D2 | D1 | D0
 * 0  | 0  | 0  | LOW_POWER |       Rate
 *
 * LOW_POWER Bit
 *
 * A setting of 0 in the LOW_POWER bit selects normal operation, and a setting 
 * of 1 selects reduced power operation, which has somewhat higher noise (see 
 * the Power Modes section for details).
 *
 * Rate Bits
 *
 * These bits select the device bandwidth and output data rate (see Table 6 and 
 * Table 7 for details). The default value is 0x0A, which translates to a 100 Hz
 * output data rate. An output data rate should be selected that is appropriate 
 * for the communication protocol and frequency selected. Selecting too high of 
 * an output data rate with a low communication speed results in samples being 
 * discarded.
 *
 * Rate (Hz)| code
 *  3200    | 1111
 *  1600    | 1110
 *   800    | 1101
 *   400    | 1100
 *   200    | 1011
 *   100    | 1010
 *    50    | 1001
 *    25    | 1000
 *    12.5  | 0111
 *     6.25 | 0110
 */
#define ADXL_BW_RATE_LO_PWR 0b00010000

#define ADXL_BW_RATE_3200   0b00001111
#define ADXL_BW_RATE_1600   0b00001110
#define ADXL_BW_RATE_0800   0b00001101
#define ADXL_BW_RATE_0400   0b00001100
#define ADXL_BW_RATE_0200   0b00001011
#define ADXL_BW_RATE_0100   0b00001010
#define ADXL_BW_RATE_0050   0b00001001
#define ADXL_BW_RATE_0025   0b00001000
#define ADXL_BW_RATE_0012   0b00000111
#define ADXL_BW_RATE_0006   0b00000110

/*******************************************************************************
 * 0x2d - Power Control
 *
 * D7 | D6 |  D5  |     D4     |   D3    |  D2   | D1 | D0
 *  0 |  0 | Link | AUTO_SLEEP | Measure | Sleep | Wakeup
 *
 * Link Bit
 *
 * A setting of 1 in the link bit with both the activity and inactivity 
 * functions enabled delays the start of the activity function until inactivity 
 * is detected. After activity is detected, inactivity detection begins, 
 * preventing the detection of activity. This bit serially links the activity 
 * and inactivity functions. When this bit is set to 0, the inactivity and 
 * activity functions are concurrent. Additional information can be found in the
 * Link Mode section.
 *
 * When clearing the link bit, it is recommended that the part be placed into 
 * standby mode and then set back to measurement mode with a subsequent write. 
 * This is done to ensure that the device is properly biased if sleep mode is 
 * manually disabled; otherwise, the first few samples of data after the link 
 * bit is cleared may have additional noise, especially if the device was asleep 
 * when the bit was cleared.
 *
 * AUTO_SLEEP Bit
 *
 * If the link bit is set, a setting of 1 in the AUTO_SLEEP bit sets the ADXL345
 * to switch to sleep mode when inactivity is detected (that is, when 
 * acceleration has been below the THRESH_INACT value for at least the time 
 * indicated by TIME_INACT). A setting of 0 disables automatic switching 
 * to sleep mode. See the description of the sleep bit in this section for more 
 * information.
 *
 * When clearing the AUTO_SLEEP bit, it is recommended that the part be placed 
 * into standby mode and then set back to measurement mode with a subsequent 
 * write. This is done to ensure that the device is properly biased if sleep 
 * mode is manually disabled; otherwise, the first few samples of data after the
 * AUTO_SLEEP bit is cleared may have additional noise, especially if the device
 * was asleep when the bit was cleared.
 *
 * Measure Bit
 *
 * A setting of 0 in the measure bit places the part into standby mode, and a 
 * setting of 1 places the part into measurement mode. The ADXL345 powers up in 
 * standby mode with minimum power consumption.
 *
 * Sleep Bit
 *
 * A setting of 0 in the sleep bit puts the part into the normal mode of 
 * operation, and a setting of 1 places the part into sleep mode. Sleep mode 
 * suppresses DATA_READY, stops transmission of data to FIFO, and switches the 
 * sampling rate to one specified by the wakeup bits. In sleep mode, only the 
 * activity function can be used.
 *
 * When clearing the sleep bit, it is recommended that the part be placed into 
 * standby mode and then set back to measurement mode with a subsequent write. 
 * This is done to ensure that the device is properly biased if sleep mode is 
 * manually disabled; otherwise, the first few samples of data after the sleep 
 * bit is cleared may have additional noise, especially if the device was asleep
 * when the bit was cleared.
 *
 * Wakeup Bits
 *
 * These bits control the frequency of readings in sleep mode as described in 
 * the table below:
 *
 * Setting
 * D1 | D0 | Frequency (Hz)
 *  0    0 |     8
 *  0    1 |     4
 *  1    0 |     2
 *  1    1 |     1
 */
#define ADXL_POWER_CTL_LINK     0b00100000
#define ADXL_POWER_CTL_AUTO_SLP 0b00010000
#define ADXL_POWER_CTL_MEASURE  0b00001000
#define ADXL_POWER_CTL_SLEEP    0b00000100

#define ADXL_POWER_CTL_WU_8 0b00000000
#define ADXL_POWER_CTL_WU_4 0b00000001
#define ADXL_POWER_CTL_WU_2 0b00000010
#define ADXL_POWER_CTL_WU_1 0b00000011

/*******************************************************************************
 * 0x2e - Interrupt Enable
 *
 *     D7     |      D6    |      D5    |    D4    |
 * DATA_READY | SINGLE_TAP | DOUBLE_TAP | Activity |
 *  
 *     D3     |     D2    |    D1     |   D0
 * Inactivity | FREE_FALL | Watermark | Overrun
 *  
 * Setting bits in this register to a value of 1 enables their respective 
 * functions to generate interrupts, whereas a value of 0 prevents the functions
 * from generating interrupts. The DATA_READY, watermark, and overrun bits 
 * enable only the interrupt output; the functions are always enabled. It is 
 * recommended that interrupts be configured before enabling their outputs.
 */
#define ADXL_INT_ENABLE_DATA_RDY 0b10000000
#define ADXL_INT_ENABLE_SNGL_TAP 0b01000000
#define ADXL_INT_ENABLE_DOBL_TAT 0b00100000
#define ADXL_INT_ENABLE_ACT      0b00010000
#define ADXL_INT_ENABLE_INACT    0b00001000
#define ADXL_INT_ENABLE_FF       0b00000100
#define ADXL_INT_ENABLE_WATER_MK 0b00000010
#define ADXL_INT_ENABLE_OVRUN    0b00000001

/*******************************************************************************
 * 0x2f - Interrupt Map
 *
 *     D7     |      D6    |      D5    |    D4    |
 * DATA_READY | SINGLE_TAP | DOUBLE_TAP | Activity |
 *
 *     D3     |     D2    |    D1     |   D0
 * Inactivity | FREE_FALL | Watermark | Overrun
 *    
 * Any bits set to 0 in this register send their respective interrupts to the 
 * INT1 pin, whereas bits set to 1 send their respective interrupts to the INT2 
 * pin. All selected interrupts for a given pin are OR’ed.
 */
#define ADXL_INT_MAP_DATA_INT1  0b00000000
#define ADXL_INT_MAP_DATA_INT2  0b10000000
#define ADXL_INT_MAP_STAP_INT1  0b00000000
#define ADXL_INT_MAP_STAP_INT2  0b01000000
#define ADXL_INT_MAP_DTAP_INT1  0b00000000
#define ADXL_INT_MAP_DTAP_INT2  0b00100000
#define ADXL_INT_MAP_ACT_INT1   0b00000000
#define ADXL_INT_MAP_ACT_INT2   0b00010000
#define ADXL_INT_MAP_INACT_INT1 0b00000000
#define ADXL_INT_MAP_INACT_INT2 0b00001000
#define ADXL_INT_MAP_FF_INT1    0b00000000
#define ADXL_INT_MAP_FF_INT2    0b00000100
#define ADXL_INT_MAP_WATER_INT1 0b00000000
#define ADXL_INT_MAP_WATER_INT2 0b00000010
#define ADXL_INT_MAP_OVRUN_INT1 0b00000000
#define ADXL_INT_MAP_OVRUN_INT2 0b00000001

/*******************************************************************************
 * 0x30 - Interrupt Source
 *
 *     D7     |      D6    |      D5    |    D4    |
 * DATA_READY | SINGLE_TAP | DOUBLE_TAP | Activity |
 *
 *     D3     |     D2    |    D1     |   D0
 * Inactivity | FREE_FALL | Watermark | Overrun
 *    
 * Bits set to 1 in this register indicate that their respective functions have 
 * triggered an event, whereas a value of 0 indicates that the corresponding 
 * event has not occurred. The DATA_READY, watermark, and overrun bits are 
 * always set if the corresponding events occur, regardless of the INT_ENABLE 
 * register settings, and are cleared by reading data from the DATAX, DATAY, and
 * DATAZ registers. The DATA_READY and watermark bits may require multiple 
 * reads, as indicated in the FIFO mode descriptions in the FIFO section. Other
 * bits, and the corresponding interrupts, are cleared by reading the INT_SOURCE
 * register.
 */
#define ADXL_INT_SOURCE_DATA  0b10000000
#define ADXL_INT_SOURCE_STAP  0b01000000
#define ADXL_INT_SOURCE_DTAP  0b00100000
#define ADXL_INT_SOURCE_ACT   0b00010000
#define ADXL_INT_SOURCE_INACT 0b00001000
#define ADXL_INT_SOURCE_FF    0b00000100
#define ADXL_INT_SOURCE_WATER 0b00000010
#define ADXL_INT_SOURCE_OVRUN 0b00000001

/*******************************************************************************
 * 0x31 - Data Format
 *
 *     D7     |  D6 |     D5     | D4 |    D3    |   D2    | D1 | D0
 * SELF_TEST  | SPI | INT_INVERT |  0 | FULL_RES | Justify |  Range
 *
 * The DATA_FORMAT register controls the presentation of data to Register 0x32 
 * through Register 0x37. All data, except that for the ±16 g range, must be 
 * clipped to avoid rollover.
 *
 * SELF_TEST Bit
 *
 * A setting of 1 in the SELF_TEST bit applies a self-test force to the sensor, 
 * causing a shift in the output data. A value of 0 disables the self-test 
 * force.
 *
 * SPI Bit
 *
 * A value of 1 in the SPI bit sets the device to 3-wire SPI mode, and a value 
 * of 0 sets the device to 4-wire SPI mode.
 *
 * INT_INVERT Bit
 *
 * A value of 0 in the INT_INVERT bit sets the interrupts to active high, and a 
 * value of 1 sets the interrupts to active low.
 *
 * FULL_RES Bit
 *
 * When this bit is set to a value of 1, the device is in full resolution mode, 
 * where the output resolution increases with the g range set by the range bits 
 * to maintain a 4 mg/LSB scale factor. When the FULL_RES bit is set to 0, the 
 * device is in 10-bit mode, and the range bits determine the maximum g range 
 * and scale factor.
 *
 * Justify Bit
 *
 * A setting of 1 in the justify bit selects left (MSB) justified mode, and a 
 * setting of 0 selects right justified mode with sign extension.
 *
 * Range Bits
 *
 * These bits set the g range as described in the table below:
 *
 * Setting
 * D1 | D0 | g Range
 * 0  | 0  | ±2 g
 * 0  | 1  | ±4 g
 * 1  | 0  | ±8 g
 * 1  | 1  | ±16 g
 */
#define ADXL_DATA_FORMAT_SELFTEST 0b10000000
#define ADXL_DATA_FORMAT_SPI      0b01000000
#define ADXL_DATA_FORMAT_INT_INV  0b00100000
#define ADXL_DATA_FORMAT_FULL_RES 0b00001000
#define ADXL_DATA_FORMAT_JUSTIFY  0b00000100

#define ADXL_DATA_FORMAT_RANGE_02 0b00000000
#define ADXL_DATA_FORMAT_RANGE_04 0b00000001
#define ADXL_DATA_FORMAT_RANGE_08 0b00000010
#define ADXL_DATA_FORMAT_RANGE_16 0b00000011

/*******************************************************************************
 * 0x38 - FIFO Control
 *
 *  D7 |  D6 |    D5   | D4 | D3 | D2 | D1 | D0
 * FIFO_MODE | Trigger |        Samples 
 *
 * FIFO Modes
 *  Setting
 *  D7 | D6 |  Mode   | Function
 *  0  | 0  | Bypass  | FIFO is bypassed.
 *  0  | 1  | FIFO    | FIFO collects up to 32 values and then stops collecting 
 *     |    |         | data, collecting new data only when FIFO is not full.
 *  1  | 0  | Stream  | FIFO holds the last 32 data values. When FIFO is full, 
 *     |    |         | the oldest data is overwritten with newer data.
 *  1  | 1  | Trigger | When triggered by the trigger bit, FIFO holds the last 
 *     |    |         | data samples before the trigger event and then continues 
 *     |    |         | to collect data until full. New data is collected only
 *     |    |         | when FIFO is not full.
 *
 * Trigger Bit
 *
 * A value of 0 in the trigger bit links the trigger event of trigger mode to 
 * INT1, and a value of 1 links the trigger event to INT2.
 *
 * Samples Bits
 * 
 * The function of these bits depends on the FIFO mode selected (see Table 20). 
 * Entering a value of 0 in the samples bits immediately sets the watermark 
 * status bit in the INT_SOURCE register, regardless of which FIFO mode is 
 * selected. Undesirable operation may occur if a value of 0 is used for the 
 * samples bits when trigger mode is used.
 *
 * Samples Bits Functions
 * FIFO Mode | Samples Bits Function
 *  Bypass   | None.
 *   FIFO    | Specifies how many FIFO entries are needed to trigger a watermark
 *           | interrupt.
 *  Stream   | Specifies how many FIFO entries are needed to trigger a watermark
 *           | interrupt.
 *  Trigger  | Specifies how many FIFO samples are retained in the FIFO buffer 
 *           | before a trigger event.
 */
#define ADXL_FIFO_CTL_BYPASS  0b00000000
#define ADXL_FIFO_CTL_FIFO    0b01000000
#define ADXL_FIFO_CTL_STREAM  0b10000000
#define ADXL_FIFO_CTL_TRIGGER 0b11000000

#define ADXL_FIFO_CTL_TRIG_BIT 0b00100000

#define ADXL_FIFO_CTL_SAMPLES  0b00001111

/*******************************************************************************
 * 0x39 - FIFO Status
 *
 *     D7    | D6 | D5 | D4 | D3 | D2 | D1 | D0
 * FIFO_TRIG | 0  |          Entries
 *
 * FIFO_TRIG Bit
 *
 * A 1 in the FIFO_TRIG bit corresponds to a trigger event occurring, and a 0 
 * means that a FIFO trigger event has not occurred.
 *
 * Entries Bits
 *
 * These bits report how many data values are stored in FIFO. Access to collect 
 * the data from FIFO is provided through the DATAX, DATAY, and DATAZ registers.
 * FIFO reads must be done in burst or multiple-byte mode because each FIFO 
 * level is cleared after any read (single- or multiple-byte) of FIFO. FIFO 
 * stores a maximum of 32 entries, which equates to a maximum of 33 entries 
 * available at any given time because an additional entry is available at the 
 * output filter of the device.
 */
#define ADXL_FIFO_STATUS_TRIG 0b10000000

#define ADXL_FIFO_STATUS_ENTRIES 0b00011111

void adxl345_setPowerControl(uint8_t data);
void adxl345_setDataFormat(uint8_t data);
void adxl345_setBWRate(uint8_t data);
void adxl345_initSTap(uint8_t thresh, uint8_t dur, uint8_t axes);
void adxl345_initDTap(uint8_t thresh, uint8_t dur, uint8_t latent, uint8_t window, uint8_t axes);
uint8_t adxl345_getIntrpt(void);
void adxl345_setIntrpt(uint8_t data);
void adxl345_setMap(uint8_t data);
int16_t adxl345_getXData(void);
int16_t adxl345_getYData(void);
int16_t adxl345_getZData(void);
void adxl345_getAccelData(uint8_t *buf);

#endif /* _ADXL345_H */
