/* ******************************************************
    Send data to MegunoLink for plotting. 
    This program reads the first three digital input values
    and sends them to MegunoLink for plotting. The values are
    separated to make them easier to visualize in the plot.
    
    Download MegunoLink, a free tool for uploading programs,
    monitoring, logging and plotting Arduino data, from: 
    http://www.MegunoLink.com/Download_MegunoLink
   ****************************************************** */

#include <GraphSeries.h>

GraphSeries g_aGraphs[] = {"Digital0", "Digital1", "Digital2"};

void setup()
{
  // The data is sent via the serial port. Initialize it.
  Serial.begin(9600);
  
  // Configure first 3  pins as digital input.
  for(int iPin = 0; iPin < 3; ++iPin)
    pinMode(iPin, INPUT);
}


void loop()
{
  int iPin;

  // Read digital pins and send them to the serial port for plotting.
  // offset each so that they are separated nicely on the graph.
  for (iPin = 0; iPin < 3; ++iPin)
    g_aGraphs[iPin].SendData((digitalRead(iPin) == HIGH ? 1 : 0) + 2 * iPin);

  delay(300);
}
