/* ******************************************************
    Send data to MegunoLink for plotting. 
    This program generates a sine and cosine signal and
    sends them out the serial port for plotting.
    
    Download MegunoLink, a free tool for uploading programs,
    monitoring, logging and plotting Arduino data, from: 
    http://www.MegunoLink.com/Download_MegunoLink
   ****************************************************** */

#include <GraphSeries.h>

GraphSeries g_Sine = GraphSeries("Sine"),
            g_Cosine = GraphSeries("Cosine");

float g_fPhase; // Keep track of signal phase.

void setup()
{
  // The data is sent via the serial port. Initialize it.
  Serial.begin(9600);
  
  g_fPhase = 0;
}


void loop()
{
  g_Sine.SendData(200*(sin(g_fPhase) + 1));
  g_Cosine.SendData(200*(cos(g_fPhase) + 1));
  g_fPhase += 0.1;

  delay(300);
}
