
#ifndef GRAPH_H
#define GRAPH_H

//#include <avr/pgmspace.h>
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class HardwareSerial;

class GraphSeries
{
  const char *m_pchSeriesName;
  
  public:
    GraphSeries(const char *pchSeriesName);
    void SendData(float fData);
    void SendData(HardwareSerial *pPort, float fData);

    static void SendData(const char *pchSeriesName, float fData);
    static void SendData(HardwareSerial *pPort, const char *pchSeriesName, float fData);

    static void SendData(prog_char *pchSeriesName, float fData);
    static void SendData(HardwareSerial *pPort, prog_char *pchSeriesName, float fData);
};

#endif
