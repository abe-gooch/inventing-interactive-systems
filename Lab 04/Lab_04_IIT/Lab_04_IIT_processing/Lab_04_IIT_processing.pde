import processing.serial.*;
import ddf.minim.*;

Serial myPort;
Minim minim;
AudioPlayer player;

void setup() {
  myPort = new Serial(this, "COM6", 115200); 
  minim = new Minim(this);
  player = minim.loadFile("thats_right.mp3");
}

void draw() {
  while (myPort.available() > 0) {
    int square_sense = myPort.read();
    println(square_sense);
    if (square_sense == 2) {
      player.rewind();
      player.play(); // Plays through your laptop speakers
    }
  }
}
