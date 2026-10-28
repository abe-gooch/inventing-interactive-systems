================= DOCUMENTATION ================================
Youtube Link: https://youtu.be/g29h_7lwKYE?si=837PQsqPylKHijnt 

================ WHAT IS THE DEVICE ==============================
The intended interaction of this device is for the user to ultimately place every shape into the square hole.
It emulates one of those kid toys meant to teach shape associations.

Meme reference: https://www.youtube.com/watch?v=6pDH66X3ClA
This project is a reference to the above meme, in which a man repeatedly puts any and all 
shapes into the square hole of a childrens toy, despite that not being where they are 
intended to go.

================== HOW DOES IT WORK =============================
The square hole has aluminum foil clipped into an alligator hook at the bottom (the other two holes have nothing), 
which runs into a touch pin on the ESP32. Each shape the user maneuvers has copper tape running across it so as to 
trigger the capacitance when placed in the square hole. 

The code uses the CalibratedAverageTouchSensor.h file provided on GitHub to calibrate the pin. It writes certain codes
to the Serial, that are then read via Processing to trigger each audio depending on the user's inputs. 

================= How TO RUN THE PROGRAM =========================
Connect the square hole sensor to pin 4 on the ESP32. Upload the code and allow it to calibrate. Do not open the Serial Monitor.
Once ready, run the Processing file ("Lab_04 IIT_processing") to enable audio playback based on user inputs.

================= AI USE ========================================
I used the AI overview on a google search to setup the core Processing code (catching stuff from the Serial, and playing audio),
and expanded it to include everything I needed.
