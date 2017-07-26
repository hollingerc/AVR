/*
 * File:      itg3205.h
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
#ifndef _ITG3205_H_
#define _ITG3205_H_ 1

/*
 * Register 0 – Who Am I
 */
uint8_t itg3205_getWhoAmI(void);

/*
 * Register 21 – Sample Rate Divider
 */
void itg3205_setSampleRate(uint8_t rate);

/*
 * Register 22 – DLPF, Full Scale
 */
#define ITG3205_FS_SEL     0b00011000

#define ITG3205_DLPF_256HZ 0b00000000
#define ITG3205_DLPF_188HZ 0b00000001
#define ITG3205_DLPF_98HZ  0b00000010
#define ITG3205_DLPF_42HZ  0b00000011
#define ITG3205_DLPF_20HZ  0b00000100
#define ITG3205_DLPF_10HZ  0b00000101
#define ITG3205_DLPF_5HZ   0b00000110

void itg3205_setDlpfScale(uint8_t data);

/*
 * Register 23 – Interrupt Configuration
 */
#define ITG3205_INT_CNFG_LEVL_ACTVHI  0b00000000
#define ITG3205_INT_CNFG_LEVL_ACTVLO  0b10000000

#define ITG3205_INT_CNFG_DRV_PSHPL    0b00000000
#define ITG3205_INT_CNFG_DRV_OD       0b01000000

#define ITG3205_INT_CNFG_LCHMD_PULSE  0b00000000
#define ITG3205_INT_CNFG_LCHMD_INTCLR 0b00100000

#define ITG3205_INT_CNFG_LCHCLR_STRD  0b00000000
#define ITG3205_INT_CNFG_LCHCLR_ANYRD 0b00010000

#define ITG3205_INT_CNFG_EI_DEV       0b00000100

#define ITG3205_INT_CNFG_EI_DATA      0b00000001

void itg3205_setInterruptConfig(uint8_t data);
uint8_t itg3205_getInterruptStatus(void);

/*
 * Registers 27 to 34 – Sensor Registers
 */
int16_t itg3205_getXData(void);
int16_t itg3205_getYData(void);
int16_t itg3205_getZData(void);
void itg3205_getGyroData(uint8_t *buf);
int16_t itg3205_getTempData(void);

/*
 * Register 62 – Power Management
 */
#define ITG3205_PWR_MGMT_RESET 0b10000000
#define ITG3205_PWR_MGMT_SLEEP 0b01000000
#define ITG3205_PWR_MGMT_XSTBY 0b00100000
#define ITG3205_PWR_MGMT_YSTBY 0b00010000
#define ITG3205_PWR_MGMT_ZSTBY 0b00001000

#define ITG3205_PWR_MGMT_CKINT    0b00000000
#define ITG3205_PWR_MGMT_PLLX     0b00000001
#define ITG3205_PWR_MGMT_PLLY     0b00000010
#define ITG3205_PWR_MGMT_PLLZ     0b00000011
#define ITG3205_PWR_MGMT_PLLINT32 0b00000100
#define ITG3205_PWR_MGMT_PLLINT19 0b00000101

void itg3205_setPowerMgmt(uint8_t data);

#endif /* _ITG3205_H_ */
