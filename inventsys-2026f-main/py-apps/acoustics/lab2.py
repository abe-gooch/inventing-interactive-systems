'''
Records audio input using the microphone. The microphone input is 
processed using and FFT and the results are displayed on a retime spectrogram.

NOTE: Make sure you chose the correct INPUT_DEVICE, the console / terminal 
window will show the inputs you have available to you with indices (e.g., 0, 1, etc.)

You can also use the spectrogram to explore different times of audible sounds. 
For example, notice the different between a nail tap and a knuckle tap on your
laptop, or the table. Compare a clap vs. a whistle.
'''

import queue
import time
import random

import numpy as np
import sounddevice as sd
import matplotlib.pyplot as plt #used to create the spectrogram window
from matplotlib.animation import FuncAnimation #allows us to update our spectrogram plot for a 'live' feed
from scipy.signal import chirp, get_window

from colorama import init, Fore, Back, Style # for styling text to be fun

# Initialize colorama and automatically reset colors after every print statement
init(autoreset=True)

print(Fore.RED + "This text is red!")
print(Back.GREEN + "This text has a green background!")
print(Style.BRIGHT + Fore.CYAN + "This text is bright cyan!")
print("This text is back to normal because of autoreset.")

RED = '\033[31m'
GREEN = '\033[32m'
YELLOW = '\033[33m'
RESET = '\033[0m' # Resets the terminal back to its default color


# -----------------------------
# Audio configuration
# -----------------------------
SAMPLE_RATE = 96_000 #44_100 # MBpro should have 96kHz max sample rate
BLOCK_SIZE = 1024 
CHANNELS = 1

# Display settings
# the number of fft bins, so SAMPLE_RATE / 4096 will give the range of frequencies captured in a bin
FFT_SIZE = 4096  
DISPLAY_SECONDS = 10
DISPLAY_MIN_FREQUENCY = 0
DISPLAY_MAX_FREQUENCY = 22050 # at most can be SAMPLE_RATE / 2 (the nyquist frequency)

CLAP_MIN_FREQUENCY = 15000
CLAP_MAX_FREQUENCY = 20000
CLAP_THRESHOLD = -90 #turn this into an array size 6 with a threshold for each bin region to map what a clap looks like?


# Other variables #
last_print_time = 0
last_clap_time = 0
clap_detected = False
waiting_for_double = False
player_distance = 0
start_of_game = True

FREQUENCY_RESOLUTION = SAMPLE_RATE / FFT_SIZE
print("Frequency Resolution (in Hz): ")
print(FREQUENCY_RESOLUTION)
NYQUIST_FREQUENCY = SAMPLE_RATE / 2 # the useful frequency range, starting from 0 Hz
print("Nyquist Frequency: ")
print(NYQUIST_FREQUENCY) #48000, but goes beyond human hearing range, so we are only displaying to 22kHz

# Set the below to None to use the system defaults.
# Input is microphone
INPUT_DEVICE = 1 


# The callback writes microphone data here.
audio_queue = queue.Queue(maxsize=50) #separates the microphone from the fft process


# -----------------------------
# Audio callback
# -----------------------------
def audio_callback(indata, outdata, frames, time_info, status):
    #indata = microphone samples, outdata = audio sent to output device, frame = number of samples
    if status: #if there is an error, print it
        print(status)

    # Put a copy of microphone input into the queue.
    try:
        audio_queue.put_nowait(indata[:, 0].copy())
    except queue.Full: #if the queue is full, remove the oldest block and add the new one \/
        # Drop the oldest block if plotting falls behind -> visual
        try:
            audio_queue.get_nowait()
            audio_queue.put_nowait(indata[:, 0].copy())
        except queue.Empty:
            pass


# -----------------------------
# Spectrogram state
# -----------------------------
frequency_bins = np.fft.rfftfreq(FFT_SIZE, 1 / SAMPLE_RATE) # THE BINS!! --------

    #note for later: frequency = bin_number × frequency_resolution

# The minimum FFT bin we will care about for the Spectrogram display
min_bin = np.searchsorted(
    frequency_bins,
    DISPLAY_MIN_FREQUENCY,
)

# The maximum FFT bin we will care about for the Spectrogram display
max_bin = np.searchsorted(
    frequency_bins,
    DISPLAY_MAX_FREQUENCY,
)

frequency_bins = frequency_bins[min_bin:max_bin]

# Number of FFT columns shown.
column_count = max(
    1,
    int(DISPLAY_SECONDS * SAMPLE_RATE / BLOCK_SIZE),
)

# Will store the data for display in the plot
spectrogram_data = np.full(
    (len(frequency_bins), column_count),
    -100.0,
    dtype=np.float32,
)

# FFT windowing using Hann fxn which smooths the signal
window = get_window("hann", FFT_SIZE)


# Computes an FFT over a block and returns the decibel result for the range
# from min_bin to max_bin (as defined above). 
def compute_fft(block):
    """Return the positive-frequency magnitude spectrum in dB."""
    if len(block) < FFT_SIZE:
        padded = np.zeros(FFT_SIZE, dtype=np.float32)
        padded[-len(block):] = block
        block = padded
    else:
        block = block[-FFT_SIZE:]
    
    # Normalizes the input to remove DC Offset
    block = block - np.mean(block)
    
    # Returns only the positive FFT values
    # the input is the windowing function (hann as specified above) f
    # unction applied to the bloc
    spectrum = np.fft.rfft(block * window)

    # Normalize magnitude and convert to dB.
    magnitude = np.abs(spectrum) / np.sum(window) # abs converts the complex number to something usable
    magnitude_db = 20 * np.log10(np.maximum(magnitude, 1e-10))
    
    return magnitude_db[min_bin:max_bin] 


# Updates the plot with the latest spectrogram data
# This is called from out animation function down below
def update_plot(_frame):
    global spectrogram_data

    global last_print_time
    global last_clap_time
    global prev_prev_clap_time
    global clap_detected
    global double_clap

    latest_column = None


    # Process all available microphone blocks.
    while True:
        try:
            block = audio_queue.get_nowait()
        except queue.Empty:
            break

        latest_column = compute_fft(block)

        # take the average across a range of bins to determine how loud it is       # baseline is for quiet room, but my laptop fans are loud
        low = np.mean(latest_column[0:171]) # 0-4kHz         - baseline ~ -90       ~clap envelope: -78 to -60 range
        mid_low = np.mean(latest_column[171:341]) # 4-8kHz    - baseline ~ -102       ~clap envelope: -88 to -66
        mid = np.mean(latest_column[341:512]) # 8-12kHz        - baseline ~ -106       ~clap envelope: -96 to -87
        mid_high = np.mean(latest_column[512:640]) #12-16kHz   - baseline ~ -106       ~clap envelope: -93 to -60
        high = np.mean(latest_column[640:854]) #16-20kHz        - baseline ~ -103       ~clap envelope: -86 to -62
        very_high = np.mean(latest_column[854:941]) #20-22kHz   - baseline ~ -110       ~clap envelope: unchanged to -98 usually
        last_print_time = time.time()
    
        info_text.set_text( #update on spectrogram
        f"low:       {low:.2f} dB\n"
        f"mid_low:   {mid_low:.2f} dB\n"
        f"mid:       {mid:.2f} dB\n"
        f"mid_high:  {mid_high:.2f} dB\n"
        f"high:      {high:.2f} dB\n"
        f"very_high: {very_high:.2f} dB"
        )

        player_input = detect_clap(low, mid_low, mid, mid_high, high, very_high)
        if player_input is not None:
            run_game(player_input) # send either single or double clap
    

    if latest_column is not None: #only update visual if there is a new sound received
        spectrogram_data[:, :-1] = spectrogram_data[:, 1:] #moves everything one column to the left
        spectrogram_data[:, -1] = latest_column #add the data to the right-most column

        image.set_data(spectrogram_data)

    return (image, info_text)

# -----------------------------
# Start audio stream and plot
# -----------------------------
print("Available audio devices:")
print(sd.query_devices())
print("\nStarting microphone capture and repeating sweep.")
print("Close the plot window or press Ctrl+C to stop.")

fig, ax = plt.subplots(figsize=(11, 6))
#fig = the window itself
#ax = the graph in the window
# 11 x 6 in

image = ax.imshow( 
    
    spectrogram_data, #2d array is displayed as an image, where color represents magnitude
    origin="lower", #puts lower frequencies at the bottom
    aspect="auto",
    interpolation="nearest", #prevents smoothing between pixels, the bins remain visually distinct
    extent=[ #define the graph coordinates
        -DISPLAY_SECONDS,
        0,
        DISPLAY_MIN_FREQUENCY,
        DISPLAY_MAX_FREQUENCY,
    ],
    cmap="magma", #color scheme
    vmin=-90, #maps dB values to colors
    vmax=-20,
)

info_text = ax.text( #listing the averages of each bin range, for debugging purposes 
    0.02, 0.98,
    "",
    transform=ax.transAxes,
    verticalalignment="top",
    color="white",
    fontsize=10,
    bbox=dict(facecolor="black", alpha=0.6)
)

ax.set_title("Live Microphone FFT Spectrogram")
ax.set_xlabel("Time relative to now (seconds)")
ax.set_ylabel("Frequency (Hz)")

colorbar = fig.colorbar(image, ax=ax) #color bar legend
colorbar.set_label("Magnitude (dB)")

# Open the audio input and output stream and 
# attach our audio_callback
stream = sd.Stream(
    samplerate=SAMPLE_RATE,
    blocksize=BLOCK_SIZE,
    dtype="float32",
    channels=(CHANNELS, CHANNELS), 
    device=(INPUT_DEVICE, None),
    callback=audio_callback,
    latency="low",
)

# ===========
# GAME
# ============   
print()
print()
print("=== Welcome to the Dark Haul! ===")
print("You must dodge oncoming obstacles by going to the left (single clap) or right (double clap).")
print("But it's dark, so you can't see them coming, leaving it all up to chance.")
print()
print(YELLOW + "Clap once to begin." + RESET)
print()
def detect_clap(low, mid_low, mid, mid_high, high, very_high):
    global last_clap_time
    global waiting_for_double

    currTime = time.time()

    clap = ( # the approximate bin envelope of a clap
        -60 > low > -79
        and -66 > mid_low > -92
        and -87 > mid > -98
        and -83 > mid_high > -102
        and -62 > high > -93
        and -87 > very_high > -105
    )

    # Handle timeout of a possible double clap
    if waiting_for_double:
        if currTime - last_clap_time > 0.75:
            waiting_for_double = False
            # print("Single clap detected")
            return "single"

    # No clap this frame
    if not clap:
        return None

    # Ignore repeated detection of same clap
    if currTime - last_clap_time < 0.1:
        return None

    # Second clap
    if waiting_for_double:
        waiting_for_double = False
        last_clap_time = currTime
        # print("Double clap detected")
        return "double"

    # First clap
    waiting_for_double = True
    last_clap_time = currTime

    return None


def run_game(clap_type):
    global player_distance
    global start_of_game
    player_input = -1

    game_direction = random.randint(0,1)
    # print("Game direction: ", game_direction) #comment when done setting up
    if(start_of_game):
        print() 
        print()
        time.sleep(0.25)
        print(f"An obstacle is approaching! {YELLOW}Clap once to dodge left, or clap twice to dodge right{RESET}")
        start_of_game = False
        return #immediately go get input
    else:
        #sorting clap -> i could just pass it as a number into this function but I can't be bothered to change it
        if clap_type == "single":
            print("Single clap - moved left!")
            player_input = 0

        elif clap_type == "double":
            print("Double clap - moved right!")
            player_input = 1 

        # gauge correctness
        if player_input != game_direction:
            print()
            print(RED + "Oh no, you hit the obstacle!!" + RESET)
            print("Total Meters traveled: ", player_distance)
            time.sleep(0.5)
            print()
            print("=== GAME OVER ===")
            print("Clap to restart.")
            player_distance = 0
            start_of_game = True

        else:
            print("Congrats, you dodged the obstacle!")
            player_distance+=1
            print(f"Meters traveled: {GREEN}{player_distance}{RESET}")
            print("-----------------------------------------------------------")
            print()
            print()
            print(f"An obstacle is approaching! {YELLOW}Clap once to dodge left, or clap twice to dodge right{RESET}")

    return


# Open stream and run animation to update plot
try:
    with stream:
        animation = FuncAnimation(
            fig,
            update_plot,
            interval=50,
            blit=True, #only update things visually that have changed
            cache_frame_data=False,
        )

        plt.show()

except KeyboardInterrupt:
    pass

finally:
    plt.close(fig)
    print("Stopped.")


# could use these as variables for if things change and the bins need to be dynamic rather than hardcoded
# Identify the bins for the desired range of frequencies
# clap_min_bin = np.searchsorted(
#     frequency_bins,
#     CLAP_MIN_FREQUENCY,
# )
# print("Clap min bin (15kHz): ")
# print(clap_min_bin)

# clap_max_bin = np.searchsorted(
#     frequency_bins,
#     CLAP_MAX_FREQUENCY
# )
# print("Clap max bin (20kHz): ")
# print(clap_max_bin)

