/**
 * @file DebugPrint.c
 * @copyright 2022 Silicon Laboratories Inc.
 */
#include "DebugPrint.h"
#include "DebugPrintConfig.h"
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "Min2Max2.h"


static uint8_t*           m_pBuffer = 0; // must be 0 before config
static uint16_t           m_iBufferSize;
static DebugPrintPrinter  m_Printer = NULL;  // Must be NULL before config


void DebugPrintConfig(uint8_t* pBuffer, uint16_t iBufferSize, DebugPrintPrinter Printer)
{
  m_Printer = NULL;   // Safe guarding - if someone prints while re-configuring.
  m_iBufferSize = iBufferSize;
  m_pBuffer = pBuffer;
  m_Printer = Printer;
}

// Float and double are not supported as data types.
void DebugPrintf(const char* pFormat, ...)
{
  if (!m_Printer)
  {
    return;
  }

  int32_t iLength;
  va_list pArgs;
  va_start(pArgs, pFormat);
  iLength = vsnprintf((char*)m_pBuffer, m_iBufferSize, pFormat, pArgs);
  va_end(pArgs);

  iLength = Minimum2Signed(iLength, m_iBufferSize - 1); // -1 since vsnprintf ensures null termination
  if (iLength > 0)
  {
    m_Printer(m_pBuffer, iLength);
  }
}

void DebugPrint(const char* pString)
{
  if (!m_Printer)
  {
    return;
  }

  m_Printer((const uint8_t*)pString, strlen(pString));
}
