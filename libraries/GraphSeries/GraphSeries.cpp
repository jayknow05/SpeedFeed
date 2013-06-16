#include <arduino.h>
#include "GraphSeries.h"

GraphSeries::GraphSeries(const char *pchSeriesName)
{
  m_pchSeriesName = pchSeriesName;
}

void GraphSeries::SendData(float fData)
{
  SendData(&Serial, m_pchSeriesName, fData);
}

void GraphSeries::SendData(HardwareSerial *pPort, float fData)
{
  SendData(pPort, m_pchSeriesName, fData);
}

void GraphSeries::SendData(const char *pchSeriesName, float fData)
{
  SendData(&Serial, pchSeriesName, fData);
}

void GraphSeries::SendData(HardwareSerial *pPort, const char *pchSeriesName, float fData)
{
  pPort->print("{");
  pPort->print(pchSeriesName);
  pPort->print(",T,");
  pPort->print(fData);
  pPort->println("}");
}

void GraphSeries::SendData(prog_char *pchSeriesName, float fData)
{
  SendData(&Serial, pchSeriesName, fData);
}

void GraphSeries::SendData(HardwareSerial *pPort, prog_char *pchSeriesName, float fData)
{
  pPort->print("{");
  pPort->print(pchSeriesName);
  pPort->print(",T,");
  pPort->print(fData);
  pPort->println("}");
}

