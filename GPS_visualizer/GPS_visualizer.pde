import processing.serial.*;
 
Serial myPort;        // The serial port

PFont font;                       // font used to display a message
int wpLimit = 16;
int numPoints = 16;
final int SCREEN_HEIGHT = 764;    //Screen heightx
final int SCREEN_WIDTH  = 914;    //Screen width

float[] xtrack;
float[] ytrack;

float[] xtrackRaw;
float[] ytrackRaw;

float[] testline;

float[][] waypoints; 
float[] startfinish;
float[] crossline;

float xtrackMin;
float xtrackMax;
float ytrackMin;
float ytrackMax;

float yMin = 14125285.08;
float yMax = 14124142.08;
float xMax = -28070207.64;
float xMin = -28071972.72;

String outputText = " ";

PImage bg;

void setup () {
  // set the window size:
  size(SCREEN_WIDTH, SCREEN_HEIGHT);
  
  xtrack = new float[numPoints];
  ytrack = new float[numPoints];
  xtrackRaw = new float[numPoints];
  ytrackRaw = new float[numPoints];
  
  startfinish = new float[4]; 
  waypoints = new float[wpLimit][4];
  testline = new float[4];
  crossline = new float[4];

  // List all the available serial ports
  println(Serial.list());
  // I know that the first port in the serial list on my mac
  // is always my  Arduino, so I open Serial.list()[0].
  // Open whatever port is the one you're using.
  myPort = new Serial(this, Serial.list()[2], 9600);
  // don't generate a serialEvent() unless you get a newline character:
  myPort.bufferUntil('\n');
  // set inital background:
  bg = loadImage("SPK_914x764.png");
  background(bg);
}

void draw () {  
  
  //image(googlemapImage, 0, 0); 
  stroke(0,255,0);
  strokeWeight(2);
  background(bg);  
  text(outputText,10,10);
  for(int i = 1; i < numPoints; i++) {
    if ((xtrack[i]==0.0)&&(ytrack[i]==0.0)) break;
    line(xtrack[i-1],ytrack[i-1],xtrack[i],ytrack[i]);
  }
  
  stroke(0,0,255);
  for(int i=0; i<wpLimit; i++) {
    line(waypoints[i][0],waypoints[i][1],waypoints[i][2],waypoints[i][3]);
    //print(waypoints[i-1][0]);print(", "); print(waypoints[i-1][1]);print(", "); print(waypoints[i-1][2]);print(", "); println(waypoints[i-1][3]);
  }
  
  stroke(0,255,0);
  line(startfinish[0],startfinish[1],startfinish[2],startfinish[3]);
  
  stroke(255,255,0);
  line(testline[0],testline[1],testline[2],testline[3]);

  stroke(255,0,0);
  line(crossline[0],crossline[1],crossline[2],crossline[3]);
}

float myMap(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void serialEvent (Serial myPort) {
  // get the ASCII string:
  String inString = myPort.readStringUntil('\n');
  String[] splitString;
  double tempVal;
  
  if (inString != null) {
    try {
      splitString = inString.split(",");
      if (splitString[0].equals("track")) {
        float xVal = Float.parseFloat(splitString[1]);
        float yVal = Float.parseFloat(splitString[2]);
   
        for(int i=1; i<numPoints; i++) {
          xtrackRaw[i-1] = xtrackRaw[i];
          ytrackRaw[i-1] = ytrackRaw[i];    
          xtrack[i-1] = xtrack[i];
          ytrack[i-1] = ytrack[i]; 
        }    

        xtrackRaw[numPoints-1] = xVal;
        ytrackRaw[numPoints-1] = yVal;
    
        for(int i=1; i<numPoints; i++) {
          xtrack[i-1] = map(xtrackRaw[i-1], xMin, xMax, 0, width);
          ytrack[i-1] = map(ytrackRaw[i-1], yMin, yMax, 0, height);
        }
      
      
    }
    else if (splitString[0].equals("waypoint")) {
      print(inString);
      
      if ((splitString[1].equals("nan")) ||
          (splitString[2].equals("nan")) ||
          (splitString[2].equals("nan")) ||
          (splitString[2].equals("nan")));
      else{
      
        for(int i=1; i<wpLimit; i++) {
          for(int j=0; j<4; j++) {
            waypoints[i-1][j] = waypoints[i][j];
          }
        }
       
        waypoints[wpLimit-1][0] = map(Float.parseFloat(splitString[1]), xMin, xMax, 0, width);
        waypoints[wpLimit-1][1] = map(Float.parseFloat(splitString[2]), yMin, yMax, 0, height);
        waypoints[wpLimit-1][2] = map(Float.parseFloat(splitString[3]), xMin, xMax, 0, width);
        waypoints[wpLimit-1][3] = map(Float.parseFloat(splitString[4]), yMin, yMax, 0, height);
      
      }
    }
    else if (splitString[0].equals("startfinish")) {
      if ((splitString[1].equals("nan")) ||
          (splitString[2].equals("nan")) ||
          (splitString[2].equals("nan")) ||
          (splitString[2].equals("nan")));
      else{ 
        startfinish[0] = map(Float.parseFloat(splitString[1]), xMin, xMax, 0, width);
        startfinish[1] = map(Float.parseFloat(splitString[2]), yMin, yMax, 0, height);
        startfinish[2] = map(Float.parseFloat(splitString[3]), xMin, xMax, 0, width);
        startfinish[3] = map(Float.parseFloat(splitString[4]), yMin, yMax, 0, height);
      }
    }
    else if (splitString[0].equals("test")) {
               
      testline[0] = map(Float.parseFloat(splitString[1]), xMin, xMax, 0, width);
      testline[1] = map(Float.parseFloat(splitString[2]), yMin, yMax, 0, height);  
      testline[2] = map(Float.parseFloat(splitString[3]), xMin, xMax, 0, width);
      testline[3] = map(Float.parseFloat(splitString[4]), yMin, yMax, 0, height);

    }
    else if (splitString[0].equals("split")) {
      print(inString);
      outputText += "Margin: " + Float.parseFloat(splitString[1])/1000.0 + "\nSplit: " + Float.parseFloat(splitString[2])/1000.0 + "\n";
      
      crossline[0] = map(Float.parseFloat(splitString[3]), xMin, xMax, 0, width);
      crossline[1] = map(Float.parseFloat(splitString[4]), yMin, yMax, 0, height);
      crossline[2] = map(Float.parseFloat(splitString[5]), xMin, xMax, 0, width);
      crossline[3] = map(Float.parseFloat(splitString[6]), yMin, yMax, 0, height);
    }
    else if (splitString[0].equals("reset")) {
      xtrack = new float[numPoints];
      ytrack = new float[numPoints];
      xtrackRaw = new float[numPoints];
      ytrackRaw = new float[numPoints];
  
      startfinish = new float[4]; 
      waypoints = new float[wpLimit][4];
      testline = new float[4];
      crossline = new float[4];
      outputText = "";
      background(bg); 
    }
    }

    catch (NumberFormatException e) {
        print("Bit error....");
      }
      
 }
 
 else {
   // increment the horizontal position:
 }

 }

