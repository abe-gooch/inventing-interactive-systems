== CODE ==
- reads the capacitance level on three pins and checks if a finger is touching a pin via a threshold calibrated during setup()
- a swipe gesture is tracked through when pins drop below the threshold, and in what order they do so

- using this, a memory game runs, delivering directions through an LCD screen
- it displays either a left or right arrow, in which the player must then input the same thing to continue playing
- each round runs through the whole sequence of inputs
- the player knows when it is their turn to input because the LED turns on, and off again once it is the system's turn


== HOW TO RUN THE PROGRAM ==
- construct the circuit as shown in the png file in the directory folder. Note that the ESP pictured is not the exact same as the one we use, but it was the closest I could find. The wires line up as if the pins on the board were the same as our ESP32.
- simply upload the code to the ESP32, the game will begin automatically
- if the player loses the game restarts


== AI USE ==
- obtain psuedocode for implementing the time element to the swipe
- identify where I was referencing an index outside of bounds in my vector (it was causing the esp to restart the program repeatedly)
