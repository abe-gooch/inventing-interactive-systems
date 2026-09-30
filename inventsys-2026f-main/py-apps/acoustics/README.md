## Lab 2 README
- documentation link: https://youtu.be/V0UPy2cK6Yw?si=MIEbP__WTtLFfEh1 

# what the code does:
- the code uses an FFT to place incoming audio signals into bins, then calculates the magnitudes in each bin to be displayed on a live spectrogram
- this spectrogram has the average magnitude for ranges of bins displayed so you can get a better real-time gist of what the envelope for an action may look like
- using this, an envelope for a clap was developed, able to differentiate itself from a snap or other similar inputs
        (It does sometimes read tapping on the laptop as a clap, but I was unable to debug that in time)
- a time-frame checks if a double clap has been performed

- these two inputs allow the user to play the game, "The Dark Haul", which is a survival game comprised purely of chance
- instructions for this game are printed to the terminal. nothing related to the game is on the spectrogram, but it felt useful to keep it accessible to the user

# how to run the code:
- ensure all libraries are installed
- simply run the file using python lab2.py in the correct directory

# AI Use
-I used ChatGPT to explain line-by-line what capture_microphone_spectrogram.py was doing. I'm new to python and was completely overwhelmed by all the libraries and things being integrated. I took what it said about each line and tried to add comments in my own words on what I felt necessary

- I also used it to write the function to display the bins on the spectrogram so I could better see what each set of bins was averaging, as well as integrating a larger number of bin ranges than I was originally using (2)

- I referenced responses from Google's search-engine version of Gemini for a collection of smaller tasks, such as where in the file certain game functions should be called (everything essentially ended up in update_plot()), or how to color the text



# GENERAL ACOUSTICS README
# Python Setup

## Setup the Virtual Env
1. From the directory of this folder, create a virtual env using: `python3 -m venv env`
2. Activate the virtual environment: 
    - On Mac: `source env/bin/activate`. 
    - On Windows: `./env/Scripts/activate.bat`
3. Install the required packages using: `pip install -r requirements.txt`
4. When adding new packages, be sure to update the requirements.txt using: `pip freeze > requirements.txt`

## Running the script 
1. Activate the virtual environment: 
    - On Mac: `source env/bin/activate`. 
    - On Windows: `./env/Scripts/activate.bat`
2. Run the script:
        `python <script_name>.py `



## Quick Copy/Paste for my specific terminal
cd C:\Users\gooch\OneDrive\Documents\GitHub\inventing-interactive-systems\acoustics\inventsys-2026f-main\py-apps\acoustics
python -m venv env
env\Scripts\activate.bat