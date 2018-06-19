import ddf.minim.*;
import ddf.minim.analysis.*;
import ddf.minim.effects.*;
import ddf.minim.signals.*;
import ddf.minim.spi.*;
import ddf.minim.ugens.*;
import processing.serial.*;
import processing.sound.*;

SoundFile file; //Object to play music

Serial myPort;  // Create object from Serial class
String val = "60";     // Data received from the serial port
String song = "";
int bpm = 60;
PFont title;
PFont f;
boolean songSet = false;

boolean mouseOverLoad = false;

void setup(){
  size(800, 800);
  
  title = createFont("Arial",30);
  f =     createFont("Arial",16);
  
   
  // I know that the first port in the serial list on my mac
  // is Serial.list()[0].
  // On Windows machines, this generally opens COM1.
  // Open whatever port is the one you're using.
  String portName = Serial.list()[0]; //change the 0 to a 1 or 2 etc. to match your port
  myPort = new Serial(this, portName, 115200);
  
  // Set thefill for text
  fill(0);
   
}      

void draw() {
  //*******************************************************
  // UPDATE MOUSE LOCATION
  //*******************************************************
  if(mouseX > 500 && mouseX < 575 && mouseY > 125 && mouseY < 165){
    mouseOverLoad = true;
  }else{
    mouseOverLoad = false;
  }
  
  //*******************************************************
  // DRAWING FUNCTIONS
  //*******************************************************
  
  background(235);                        //set background
  
  //All buttons
  fill(255);
  rect(185,125,300,40);  //file name entry box
  if(!mouseOverLoad){    
    fill(#4cbb17);
  }else{
    fill(#4cff17);
  }
  rect(500,125,75,40);  //load button
  
  //All text
  fill(0);
  textFont(title);                        //set title font
  text("Heart Rate Music Player",230,40); //print heading
  textFont(f);                            //reset font to main
  text("Instructions: Load music files into data folder (must be of type .mp3). Enter the name of the file below \nand press the load button.",40,75);
  text("File name:     "+song,100,150);
  text("LOAD",516,150);
  
  fill(#4cbb17);
  if(songSet){
    text("Song loaded!",600,150);
  }
  
  //*******************************************************
  // DETECT BUTTON PRESSES
  //*******************************************************
  if(mousePressed && mouseOverLoad){
     songSet = loadFile(song);
  }
  
  if(!songSet){
    
     
  }else{
    if ( myPort.available() > 0) 
    {  // If data is available,
      val = myPort.readStringUntil('\n');         // read it and store it in val
    } 
    
    text("Heart BPM: "+bpm,25,40); 
    
    file.rate(float(val)/bpm);
  }
  
}

boolean loadFile(String fileName){
    file = new SoundFile(this, fileName);
    
    try{
      file.cue(0);
    }catch(NullPointerException e){
      song = "";
      return false;
    }
    return true;
}

void keyPressed() { 
  if(!songSet){
    if(key == BACKSPACE && song.length() != 0){
      song = song.substring( 0, song.length()-1 );
    }else if(song.length() < 20 && key != BACKSPACE){
      song = song + key; 
    }
  }
}
