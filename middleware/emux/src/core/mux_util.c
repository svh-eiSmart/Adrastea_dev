/*  ---------------------------------------------------------------------------

    (c) copyright 2021 Altair Semiconductor, Ltd. All rights reserved.

    This software, in source or object form (the "Software"), is the
    property of Altair Semiconductor Ltd. (the "Company") and/or its
    licensors, which have all right, title and interest therein, You
    may use the Software only in  accordance with the terms of written
    license agreement between you and the Company (the "License").
    Except as expressly stated in the License, the Company grants no
    licenses by implication, estoppel, or otherwise. If you are not
    aware of or do not agree to the License terms, you may not use,
    copy or modify the Software. You may use the source code of the
    Software only for your internal purposes and may not distribute the
    source code of the Software, any part thereof, or any derivative work
    thereof, to any third party, except pursuant to the Company's prior
    written consent.
    The Software is the confidential information of the Company.

   ------------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include "core/mux_util.h"
#include "alt_osal.h"
#include "serial_hal/hal_mux.h"

// clang-format off
#define PREAMBLE_FOUND          1
#define VIRTUAL_SER_NUM_BITS    16
#define VIRTUAL_SER_NUM_MASK    ((1 << VIRTUAL_SER_NUM_BITS) - 1)
#define DATA_LEN_BIT_COUNT      15
#define DATA_MAX_LENGTH_MUX     ((1 << DATA_LEN_BIT_COUNT) - 1) // 32767
#define HEADER_PREFIX_SIZE      5
#define HEADER_POSTFIX_SIZE     2
#define MUX_FLAG                0xF9
#define MUX_EA                  1
#define MUX_CR                  2
#define DATA_MAX_PERMITTED_LEN  1024  // in order to make MUX less prone to long random messages (won't stuck waiting for many bytes)
#define DATA_MAX_LEN_OVERALL    (DATA_MAX_PERMITTED_LEN < DATA_MAX_LENGTH_MUX ? DATA_MAX_PERMITTED_LEN : DATA_MAX_LENGTH_MUX)
#define RECEIVE_BUFFER_LEN      (DATA_MAX_LEN_OVERALL + HEADER_POSTFIX_SIZE)

#define MUX_RECEIVE_BUFF_DESC   "MUX receive buff "
#define DEFAULT_VIRTUAL_OUTPUT  3

#define HISTORY_LENGTH          10
#define UBOOT_SEARCH_TOKEN      "-Boot"
#define UBOOT_PRNT_START        "\r\nU-Boot"
#define UBOOT_STARTUP           "boot\r\n"

#define losGetTimeInMilliseconds (alt_osal_get_tick_count() / g_tick_period_ms);
// clang-format on

/* The format of simple CMUX messages is as follows:
 * [flag byte - 0xF9][serial ID - 6 bits (and 2 constant bits)][control - byte][data length - 1/2
 * bytes][data][Checksum - byte][flag byte - 0xF9]
 */

// NOTE: Define the following functions typedef in "losMux.h": muxRxFp_t, muxTxBuffFp_t
typedef struct {
  muxRxFp_t serialRxProcessFp;
  void *appCookie;
  alt_osal_mutex_handle rxSem;
} virtualMuxPort_t;

// MUX transmit semaphore
alt_osal_mutex_handle xTransmitSemaphore[MAX_MUX_COUNT] = {0};
// MUX transmit function
muxTxBuffFp_t muxTxcharF[MAX_MUX_COUNT] = {0};
// MUX transmit handler
static void *muxSerialHandle[MAX_MUX_COUNT] = {0};
virtualMuxPort_t virtualMuxPorts[MAX_MUX_COUNT][VIRTUAL_SERIAL_COUNT] = {0};

uint8_t mainReceiveBuff_0[RECEIVE_BUFFER_LEN];
#if MAX_MUX_COUNT == 2
uint8_t mainReceiveBuff_1[RECEIVE_BUFFER_LEN];
#endif

static uint32_t g_tick_period_ms = 1;

const uint8_t crcPolyTable[256] = {
    0x00, 0x91, 0xE3, 0x72, 0x07, 0x96, 0xE4, 0x75, 0x0E, 0x9F, 0xED, 0x7C, 0x09, 0x98, 0xEA, 0x7B,
    0x1C, 0x8D, 0xFF, 0x6E, 0x1B, 0x8A, 0xF8, 0x69, 0x12, 0x83, 0xF1, 0x60, 0x15, 0x84, 0xF6, 0x67,
    0x38, 0xA9, 0xDB, 0x4A, 0x3F, 0xAE, 0xDC, 0x4D, 0x36, 0xA7, 0xD5, 0x44, 0x31, 0xA0, 0xD2, 0x43,
    0x24, 0xB5, 0xC7, 0x56, 0x23, 0xB2, 0xC0, 0x51, 0x2A, 0xBB, 0xC9, 0x58, 0x2D, 0xBC, 0xCE, 0x5F,
    0x70, 0xE1, 0x93, 0x02, 0x77, 0xE6, 0x94, 0x05, 0x7E, 0xEF, 0x9D, 0x0C, 0x79, 0xE8, 0x9A, 0x0B,
    0x6C, 0xFD, 0x8F, 0x1E, 0x6B, 0xFA, 0x88, 0x19, 0x62, 0xF3, 0x81, 0x10, 0x65, 0xF4, 0x86, 0x17,
    0x48, 0xD9, 0xAB, 0x3A, 0x4F, 0xDE, 0xAC, 0x3D, 0x46, 0xD7, 0xA5, 0x34, 0x41, 0xD0, 0xA2, 0x33,
    0x54, 0xC5, 0xB7, 0x26, 0x53, 0xC2, 0xB0, 0x21, 0x5A, 0xCB, 0xB9, 0x28, 0x5D, 0xCC, 0xBE, 0x2F,
    0xE0, 0x71, 0x03, 0x92, 0xE7, 0x76, 0x04, 0x95, 0xEE, 0x7F, 0x0D, 0x9C, 0xE9, 0x78, 0x0A, 0x9B,
    0xFC, 0x6D, 0x1F, 0x8E, 0xFB, 0x6A, 0x18, 0x89, 0xF2, 0x63, 0x11, 0x80, 0xF5, 0x64, 0x16, 0x87,
    0xD8, 0x49, 0x3B, 0xAA, 0xDF, 0x4E, 0x3C, 0xAD, 0xD6, 0x47, 0x35, 0xA4, 0xD1, 0x40, 0x32, 0xA3,
    0xC4, 0x55, 0x27, 0xB6, 0xC3, 0x52, 0x20, 0xB1, 0xCA, 0x5B, 0x29, 0xB8, 0xCD, 0x5C, 0x2E, 0xBF,
    0x90, 0x01, 0x73, 0xE2, 0x97, 0x06, 0x74, 0xE5, 0x9E, 0x0F, 0x7D, 0xEC, 0x99, 0x08, 0x7A, 0xEB,
    0x8C, 0x1D, 0x6F, 0xFE, 0x8B, 0x1A, 0x68, 0xF9, 0x82, 0x13, 0x61, 0xF0, 0x85, 0x14, 0x66, 0xF7,
    0xA8, 0x39, 0x4B, 0xDA, 0xAF, 0x3E, 0x4C, 0xDD, 0xA6, 0x37, 0x45, 0xD4, 0xA1, 0x30, 0x42, 0xD3,
    0xB4, 0x25, 0x57, 0xC6, 0xB3, 0x22, 0x50, 0xC1, 0xBA, 0x2B, 0x59, 0xC8, 0xBD, 0x2C, 0x5E, 0xCF};

uint8_t calcFCS(const uint8_t *input, int32_t count, const uint8_t *data, int32_t dataCount) {
  uint8_t fcs = 0xFF;
  int32_t i;

  for (i = 0; i < count; i++) {
    fcs = crcPolyTable[fcs ^ input[i]];
  }

  for (i = 0; i < dataCount; i++) {
    fcs = crcPolyTable[fcs ^ data[i]];
  }

  return (0xFF - fcs);
}

int32_t isReadableChar(uint8_t c) {
  // all chars between those are valid characters
  if (((c >= ' ') && (c <= '~')) || (c == '\r') || (c == '\n') || (c == '\b')) {
    return 1;
  } else {
    return 0;
  }
}

int32_t transmitOnMux(halMuxHdl_t *muxHandle, const uint8_t *txCharBuf, uint16_t txCharBufLen) {
  // char header[HEADER_SIZE];
  uint8_t headerPrefix[HEADER_PREFIX_SIZE] = {MUX_FLAG, MUX_EA | MUX_CR, 0, 0, 0};
  uint8_t headerPostfix[HEADER_POSTFIX_SIZE] = {0, MUX_FLAG};
  int32_t prefixLen = 4;  // can be 4 or 5
  uint16_t idx = 0;
  uint16_t currentLen, remainingLen;
  int32_t muxID = ((int32_t)muxHandle - 1) >> VIRTUAL_SER_NUM_BITS;
  int32_t virtualSerID = ((int32_t)muxHandle - 1) & VIRTUAL_SER_NUM_MASK;

  if ((muxID < 0) || (muxID >= MAX_MUX_COUNT)) {
    return 0;
  }
  if ((virtualSerID < 0) || (virtualSerID >= VIRTUAL_SERIAL_COUNT)) {
    return 0;
  }
  if (txCharBufLen == 0) {
    return 0;
  }

  headerPrefix[1] = headerPrefix[1] | ((0x3F & (uint8_t)virtualSerID) << 2);
  while (idx < txCharBufLen) {
    // break the data to smaller messages sizes
    remainingLen = txCharBufLen - idx;
    currentLen = (remainingLen < DATA_MAX_LEN_OVERALL) ? remainingLen : DATA_MAX_LEN_OVERALL;
    // build header
    // length
    if (currentLen > 127) {
      prefixLen = 5;
      headerPrefix[3] = ((0x7F & currentLen) << 1);
      headerPrefix[4] = currentLen >> 7;
    } else {
      prefixLen = 4;
      headerPrefix[3] = 1 | (currentLen << 1);
    }

    headerPostfix[0] =
        calcFCS(headerPrefix + 1, prefixLen - 1, &(((uint8_t *)txCharBuf)[idx]), currentLen);

    if (muxTxcharF[muxID] != NULL) {
      alt_osal_lock_mutex(&(xTransmitSemaphore[muxID]), ALT_OSAL_TIMEO_FEVR);

      muxTxcharF[muxID](muxSerialHandle[muxID], (uint8_t *)headerPrefix, prefixLen);
      muxTxcharF[muxID](muxSerialHandle[muxID], &(txCharBuf[idx]), currentLen);
      muxTxcharF[muxID](muxSerialHandle[muxID], (uint8_t *)headerPostfix, HEADER_POSTFIX_SIZE);

      alt_osal_unlock_mutex(&(xTransmitSemaphore[muxID]));
    }

    idx += currentLen;
  }

  return (int32_t)idx;
}

int32_t bindToMuxVirtualPort(int32_t muxID, int32_t virtualSerID, muxRxFp_t serialRxProcessFp,
                             void *appCookie, muxTxBuffFp_t *serialTxcharFp) {
  int32_t retHandle;

  if ((muxID < 0) || (muxID >= MAX_MUX_COUNT)) {
    return 0;
  }
  if ((virtualSerID < 0) || (virtualSerID >= VIRTUAL_SERIAL_COUNT)) {
    return 0;
  }

  retHandle = ((muxID << VIRTUAL_SER_NUM_BITS) | virtualSerID) + 1;

  if (virtualMuxPorts[muxID][virtualSerID].serialRxProcessFp != NULL) {
    MUX_DBG("MUX virtual serial is already taken\n");
    return retHandle;
  }

  if (serialTxcharFp == NULL) {
    MUX_DBG("Can't set transmit function, pointer is null\n");
    return 0;
  }

  virtualMuxPorts[muxID][virtualSerID].serialRxProcessFp = serialRxProcessFp;
  virtualMuxPorts[muxID][virtualSerID].appCookie = appCookie;

  *serialTxcharFp = transmitOnMux;

  return retHandle;
}

int32_t unbindFromMuxVirtualPort(int32_t muxID, int32_t virtualSerID) {
  if ((muxID < 0) || (muxID >= MAX_MUX_COUNT)) {
    return 0;
  }
  if ((virtualSerID < 0) || (virtualSerID >= VIRTUAL_SERIAL_COUNT)) {
    return 0;
  }

  alt_osal_lock_mutex(&(virtualMuxPorts[muxID][virtualSerID].rxSem), ALT_OSAL_TIMEO_FEVR);
  virtualMuxPorts[muxID][virtualSerID].serialRxProcessFp = NULL;
  virtualMuxPorts[muxID][virtualSerID].appCookie = NULL;
  alt_osal_unlock_mutex(&(virtualMuxPorts[muxID][virtualSerID].rxSem));

  return 1;
}

void writeStringToVirtualSerial(int32_t muxID, int32_t virtualSerID, const uint8_t *string) {
  int32_t i;
  int32_t strLen = strlen((const char *)string);
  void *serCookie = virtualMuxPorts[muxID][virtualSerID].appCookie;

  if (virtualMuxPorts[muxID][virtualSerID].serialRxProcessFp != 0) {
    for (i = 0; i < strLen; i++) {
      virtualMuxPorts[muxID][virtualSerID].serialRxProcessFp(string[i], serCookie);
    }
  }
}

void muxReceiveCallback_MUX_0(const uint8_t charRecv, void *cookie) {
  static int32_t preambleFound = !PREAMBLE_FOUND;
  static int32_t virtualSer;
  static int32_t dataToRead = 0;
  static int32_t headerRead = 0;
  uint8_t *dataBuffer = mainReceiveBuff_0;
  static int32_t idx = 0;
  static uint8_t header[HEADER_PREFIX_SIZE] = {MUX_FLAG, 0, 0, 0, 0};
  static uint8_t history[HISTORY_LENGTH] = {0};
  static int32_t historyLen = 0;
  uint8_t checksum;
  static int32_t muxID = 0;  // This function is a state machine for MUX 0
  void *serCookie;
  static int32_t deviceAtUboot = 0;
  int32_t i;
  char escSeqOne[] = {ESC_SEQ_PART_ONE, 0};
  char escSeqTwo[] = {ESC_SEQ_PART_TWO, 0};
  static uint32_t escOneFirstAppearance = 0, escTwoFirstAppearance, escDelta;

  // accumulate chars into the history buffer
  if (historyLen == HISTORY_LENGTH) {
    for (i = 0; i < HISTORY_LENGTH - 1; i++) {
      history[i] = history[i + 1];
    }
    history[historyLen - 1] = charRecv;
  } else {
    history[historyLen] = charRecv;
    historyLen++;
  }

  // Search for the first escape sequence part
  if (strstr((const char *)history, escSeqOne)) {
    escOneFirstAppearance = losGetTimeInMilliseconds;
    // reset history
    memset(history, 0, HISTORY_LENGTH);
    historyLen = 0;
    /*MUX_DBG("receive esc one\r\n");*/
  }

  // Search for the second escape sequence part
  if (strstr((char *)history, escSeqTwo)) {
    escTwoFirstAppearance = losGetTimeInMilliseconds;
    escDelta = escTwoFirstAppearance - escOneFirstAppearance;
    /*MUX_DBG("receive esc two. escDelta:%ld\r\n", escDelta);*/
    if ((ESC_SEQ_DELAY_LOWER_THRESH <= escDelta) && (escDelta <= ESC_SEQ_DELAY_UPPER_THRESH)) {
      // Found full escape sequence
      // reset state of this function (static variables)
      /*MUX_DBG("receive escape sequence\r\n");*/
      memset(history, 0, HISTORY_LENGTH);
      historyLen = 0;
      dataToRead = 0;
      idx = 0;
      preambleFound = !PREAMBLE_FOUND;
      return;
    }
    memset(history, 0, HISTORY_LENGTH);
    historyLen = 0;
  }

  if (dataToRead > 0) {
    dataBuffer[idx] = charRecv;
    idx++;
    if (idx == dataToRead + HEADER_POSTFIX_SIZE) {
      // all data and postfix of header has been read
      checksum = calcFCS(header + 1, headerRead - 1, dataBuffer, dataToRead);
      if ((dataBuffer[idx - 1] == MUX_FLAG) && (dataBuffer[idx - 2] == checksum)) {
        // all header and data is valid
        deviceAtUboot = 0;
        virtualSer = header[1] >> 2;
        alt_osal_lock_mutex(&(virtualMuxPorts[muxID][virtualSer].rxSem), ALT_OSAL_TIMEO_FEVR);
        if (virtualMuxPorts[muxID][virtualSer].serialRxProcessFp != 0) {
          serCookie = virtualMuxPorts[muxID][virtualSer].appCookie;
          for (idx = 0; idx < dataToRead; idx++) {
            virtualMuxPorts[muxID][virtualSer].serialRxProcessFp(((uint8_t *)dataBuffer)[idx],
                                                                 serCookie);
          }
        }
        alt_osal_unlock_mutex(&(virtualMuxPorts[muxID][virtualSer].rxSem));
      } else {
        MUX_DBG("== Invalid packet, throwing it ==\n");

        MUX_DBG("idx:%ld, dataToRead:%ld, flag:%02X, checksum:%02X, calc checksum:%02X\n", idx,
                dataToRead, dataBuffer[idx - 1], dataBuffer[idx - 2], checksum);
        MUX_DBG("buffLen:%d, buffer:\n", strlen((const char *)dataBuffer));
        int32_t td;
        for (td = 0; td < dataToRead + HEADER_POSTFIX_SIZE + 6; td++) {
          MUX_DBG("%02X", dataBuffer[td]);
        }
        MUX_DBG("\n============\n");
      }
      dataToRead = 0;
      idx = 0;
      preambleFound = !PREAMBLE_FOUND;
    }
    return;
  }

  // state machine for parsing MUX format messages
  if (preambleFound == PREAMBLE_FOUND) {
    if ((headerRead == 1) && ((uint8_t)charRecv == MUX_FLAG)) {
      // the FLAG can't appear twice in the start of the header,
      // this means that we now found a new start
      return;
    }
    if (headerRead >= HEADER_PREFIX_SIZE) {
      // This shouldn't happen, reset the state of this state machine
      MUX_DBG("Error: got too much of the header bytes, re-searching\n");
      dataToRead = 0;
      headerRead = 0;
      preambleFound = !PREAMBLE_FOUND;
      return;
    }
    header[headerRead] = charRecv;
    headerRead++;

    // header size may be 4 or 5
    if (headerRead == 4) {  // if got 4 bytes of header, check if valid
      // if the first bit in the byte is 1, then only 1 byte of length is used
      if (header[3] & 1) {
        dataToRead = header[3] >> 1;
        if ((dataToRead == 0) || (dataToRead > DATA_MAX_LEN_OVERALL)) {
          // invalid packet, drop it
          MUX_DBG("Drop invalid packet, data length:%ld (max permitted: %d)\n", dataToRead,
                  DATA_MAX_LEN_OVERALL);
          dataToRead = 0;
          headerRead = 0;
          preambleFound = !PREAMBLE_FOUND;
        } else {
        }
        return;
      }
    }

    if (headerRead == 5) {
      dataToRead = (header[4] << 7) + (header[3] >> 1);
      if ((dataToRead == 0) || (dataToRead > DATA_MAX_LEN_OVERALL)) {
        MUX_DBG("Drop invalid packet, data length:%ld (max permitted: %d)\n", dataToRead,
                DATA_MAX_LEN_OVERALL);
        dataToRead = 0;
        headerRead = 0;
        preambleFound = !PREAMBLE_FOUND;
      } else {
      }
      return;
    }
  } else {
    if ((uint8_t)charRecv == MUX_FLAG) {
      preambleFound = PREAMBLE_FOUND;
      headerRead = 1;
    } else {
      if (!deviceAtUboot && strstr((char *)history, UBOOT_SEARCH_TOKEN)) {
        writeStringToVirtualSerial(muxID, DEFAULT_VIRTUAL_OUTPUT, (uint8_t *)UBOOT_PRNT_START);
        deviceAtUboot = 1;
        return;
      }
      if (deviceAtUboot) {  // as long as it is in boot
        // if the packet preamble is not yet found (not collecting packet header/data), send any
        // readable chars to a default port
        if (isReadableChar(charRecv)) {
          if (charRecv == '#') {
            // if talking to u-boot and it's stuck, send boot command
            muxTxcharF[muxID](muxSerialHandle[muxID], (const uint8_t *)UBOOT_STARTUP,
                              strlen(UBOOT_STARTUP));
          }

          // transmit any readable chars to the default virtual port
          if (virtualMuxPorts[muxID][DEFAULT_VIRTUAL_OUTPUT].serialRxProcessFp != 0) {
            serCookie = virtualMuxPorts[muxID][DEFAULT_VIRTUAL_OUTPUT].appCookie;
            virtualMuxPorts[muxID][DEFAULT_VIRTUAL_OUTPUT].serialRxProcessFp(charRecv, serCookie);
          }
        }
      }
    }
  }
}
#if MAX_MUX_COUNT == 2
void muxReceiveCallback_MUX_1(const uint8_t charRecv, void *cookie) {
  static int32_t preambleFound = !PREAMBLE_FOUND;
  static int32_t virtualSer;
  static int32_t dataToRead = 0;
  static int32_t headerRead = 0;
  uint8_t *dataBuffer = mainReceiveBuff_1;
  static int32_t idx = 0;
  static uint8_t header[HEADER_PREFIX_SIZE] = {MUX_FLAG, 0, 0, 0, 0};
  static uint8_t history[HISTORY_LENGTH] = {0};
  static int32_t historyLen = 0;
  uint8_t checksum;
  static int32_t muxID = 1;  // This function is a state machine for MUX 1
  void *serCookie;
  static int32_t deviceAtUboot = 0;
  int32_t i;
  char escSeqOne[] = {ESC_SEQ_PART_ONE, 0};
  char escSeqTwo[] = {ESC_SEQ_PART_TWO, 0};
  static uint32_t escOneFirstAppearance = 0, escTwoFirstAppearance, escDelta;

  // accumulate chars into the history buffer
  if (historyLen == HISTORY_LENGTH) {
    for (i = 0; i < HISTORY_LENGTH - 1; i++) {
      history[i] = history[i + 1];
    }
    history[historyLen - 1] = charRecv;
  } else {
    history[historyLen] = charRecv;
    historyLen++;
  }

  // Search for the first escape sequence part
  if (strstr((const char *)history, escSeqOne)) {
    escOneFirstAppearance = losGetTimeInMilliseconds;
    // reset history
    memset(history, 0, HISTORY_LENGTH);
    historyLen = 0;
  }

  // Search for the second escape sequence part
  if (strstr((const char *)history, escSeqTwo)) {
    escTwoFirstAppearance = losGetTimeInMilliseconds;
    escDelta = escTwoFirstAppearance - escOneFirstAppearance;
    if ((ESC_SEQ_DELAY_LOWER_THRESH <= escDelta) && (escDelta <= ESC_SEQ_DELAY_UPPER_THRESH)) {
      // Found full escape sequence
      // reset state of this function (static variables)
      memset(history, 0, HISTORY_LENGTH);
      historyLen = 0;
      dataToRead = 0;
      idx = 0;
      preambleFound = !PREAMBLE_FOUND;
      return;
    }
    memset(history, 0, HISTORY_LENGTH);
    historyLen = 0;
  }

  if (dataToRead > 0) {
    dataBuffer[idx] = charRecv;
    idx++;
    if (idx == dataToRead + HEADER_POSTFIX_SIZE) {
      // all data and postfix of header has been read
      checksum = calcFCS(header + 1, headerRead - 1, dataBuffer, dataToRead);
      if ((dataBuffer[idx - 1] == MUX_FLAG) && (dataBuffer[idx - 2] == checksum)) {
        // all header and data is valid
        virtualSer = header[1] >> 2;
        deviceAtUboot = 0;
        alt_osal_lock_mutex(&(virtualMuxPorts[muxID][virtualSer].rxSem), ALT_OSAL_TIMEO_FEVR);
        if (virtualMuxPorts[muxID][virtualSer].serialRxProcessFp != 0) {
          serCookie = virtualMuxPorts[muxID][virtualSer].appCookie;
          for (idx = 0; idx < dataToRead; idx++) {
            virtualMuxPorts[muxID][virtualSer].serialRxProcessFp(((uint8_t *)dataBuffer)[idx],
                                                                 serCookie);
          }
        }
        alt_osal_unlock_mutex(&(virtualMuxPorts[muxID][virtualSer].rxSem));
      } else {
        MUX_DBG("== Invalid packet, throwing it ==\n");
        MUX_DBG("idx:%d, dataToRead:%d, flag:%02X, checksum:%02X, calc checksum:%02X\n", idx,
                dataToRead, dataBuffer[idx - 1], dataBuffer[idx - 2], checksum);
        MUX_DBG("buffLen:%d, buffer:\n", strlen(dataBuffer));
        int td;
        for (td = 0; td < dataToRead + HEADER_POSTFIX_SIZE + 6; td++) {
          MUX_DBG("%02X", dataBuffer[td]);
        }
        MUX_DBG("\n============\n");
      }
      dataToRead = 0;
      idx = 0;
      preambleFound = !PREAMBLE_FOUND;
    }
    return;
  }

  // state machine for parsing MUX format messages
  if (preambleFound == PREAMBLE_FOUND) {
    if ((headerRead == 1) && ((uint8_t)charRecv == MUX_FLAG)) {
      // the FLAG can't appear twice in the start of the header,
      // this means that we now found a new start
      return;
    }
    if (headerRead >= HEADER_PREFIX_SIZE) {
      // This shouldn't happen, reset the state of this state machine
      MUX_DBG("Error: got too much of the header bytes, re-searching\n");
      dataToRead = 0;
      headerRead = 0;
      preambleFound = !PREAMBLE_FOUND;
      return;
    }
    header[headerRead] = charRecv;
    headerRead++;

    // header size may be 4 or 5
    if (headerRead == 4) {  // if got 4 bytes of header, check if valid
      // if the first bit in the byte is 1, then only 1 byte of length is used
      if (header[3] & 1) {
        dataToRead = header[3] >> 1;
        if ((dataToRead == 0) || (dataToRead > DATA_MAX_LEN_OVERALL)) {
          MUX_DBG("Drop invalid packet, data length:%ld (max permitted: %d)\n", dataToRead,
                  DATA_MAX_LEN_OVERALL);
          // invalid packet, drop it
          dataToRead = 0;
          headerRead = 0;
          preambleFound = !PREAMBLE_FOUND;
        } else {
        }
        return;
      }
    }

    if (headerRead == 5) {
      dataToRead = (header[4] << 7) + (header[3] >> 1);
      if ((dataToRead == 0) || (dataToRead > DATA_MAX_LEN_OVERALL)) {
        MUX_DBG("Drop invalid packet, data length:%ld (max permitted: %d)\n", dataToRead,
                DATA_MAX_LEN_OVERALL);
        // invalid packet, drop it
        dataToRead = 0;
        headerRead = 0;
        preambleFound = !PREAMBLE_FOUND;
      } else {
      }
      return;
    }
  } else {
    if ((uint8_t)charRecv == MUX_FLAG) {
      preambleFound = PREAMBLE_FOUND;
      headerRead = 1;
    } else {
      if (!deviceAtUboot && strstr((const char *)history, UBOOT_SEARCH_TOKEN)) {
        writeStringToVirtualSerial(muxID, DEFAULT_VIRTUAL_OUTPUT,
                                   (const uint8_t *)UBOOT_PRNT_START);
        deviceAtUboot = 1;
        return;
      }
      if (deviceAtUboot) {  // as long as it is in boot
        // if the packet preamble is not yet found (not collecting packet header/data), send any
        // readable chars to a default port
        if (isReadableChar(charRecv)) {
          if (charRecv == '#') {
            // if talking to u-boot and it's stuck, send boot command
            muxTxcharF[muxID](muxSerialHandle[muxID], (const uint8_t *)UBOOT_STARTUP,
                              strlen(UBOOT_STARTUP));
          }

          // transmit any readable chars to the default virtual port
          if (virtualMuxPorts[muxID][DEFAULT_VIRTUAL_OUTPUT].serialRxProcessFp != 0) {
            serCookie = virtualMuxPorts[muxID][DEFAULT_VIRTUAL_OUTPUT].appCookie;
            virtualMuxPorts[muxID][DEFAULT_VIRTUAL_OUTPUT].serialRxProcessFp(charRecv, serCookie);
          }
        }
      }
    }
  }
}
#endif

void muxReceiveCallback(const uint8_t charRecv, void *cookie) {
  int32_t muxID = (int32_t)cookie;

  if (muxID == 0) {
    muxReceiveCallback_MUX_0(charRecv, cookie);
  } else if (muxID == 1) {
#if MAX_MUX_COUNT == 2
    muxReceiveCallback_MUX_1(charRecv, cookie);
#endif
  }
}

int32_t createMux(int32_t muxID, int32_t numberOfVirtualPorts) {
  int32_t i;
  halMuxHdl_t halHandle;
  alt_osal_mutex_attribute mutex_param = {0};
  if ((muxID < 0) || (muxID >= MAX_MUX_COUNT)) {
    return ERROR;
  }

  g_tick_period_ms = 1000 / alt_osal_get_tick_freq();

  if (alt_osal_create_mutex(&(xTransmitSemaphore[muxID]), &mutex_param) != 0) goto mux_err;

  for (i = 0; i < numberOfVirtualPorts; i++) {
    virtualMuxPorts[muxID][i].serialRxProcessFp = NULL;
    virtualMuxPorts[muxID][i].appCookie = NULL;
    if (alt_osal_create_mutex(&(virtualMuxPorts[muxID][i].rxSem), &mutex_param) != 0) goto mux_err;
  }
  if ((halHandle = halSerialConfigure(muxID, muxReceiveCallback, (void *)muxID,
                                      &muxTxcharF[muxID])) != NULL) {
    muxSerialHandle[muxID] = halHandle;
    return 0;
  }

mux_err:
  if (xTransmitSemaphore[muxID]) alt_osal_delete_mutex(&(xTransmitSemaphore[muxID]));
  for (i = 0; i < numberOfVirtualPorts; i++) {
    if (virtualMuxPorts[muxID][i].rxSem) alt_osal_delete_mutex(&(virtualMuxPorts[muxID][i].rxSem));
  }
  return ERROR;
}
