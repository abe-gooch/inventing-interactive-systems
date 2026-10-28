import processing.serial.*;
import ddf.minim.*;

Serial myPort;
Minim minim;
AudioPlayer thats_right;
AudioPlayer squareQ;
AudioPlayer circleQ;
AudioPlayer triQ;

void setup() {
  myPort = new Serial(this, "COM6", 115200); 
  minim = new Minim(this);
  thats_right = minim.loadFile("thats_right.mp3");
  squareQ = minim.loadFile("squareQ.mp3");
  circleQ = minim.loadFile("circleQ.mp3");
  triQ = minim.loadFile("triQ.mp3");
}

void draw() {
  while (myPort.available() > 0) {
    int square_sense = myPort.read();
    if(square_sense == 1){ // trigger square question
      println(square_sense);
      squareQ.rewind();
      squareQ.play();
    }
    else if (square_sense == 2) { // trigger "that's right"
      println(square_sense);
      thats_right.rewind();
      thats_right.play(); // Plays through your laptop speakers
    }
    else if (square_sense == 3) { // trigger circle question
      println(square_sense);
      circleQ.rewind();
      circleQ.play();
    }
    else if (square_sense == 4) { // trigger triangle question
      println(square_sense);
      triQ.rewind();
      triQ.play();
    }
  }
}
