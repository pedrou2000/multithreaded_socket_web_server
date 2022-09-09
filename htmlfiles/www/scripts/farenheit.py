import sys
import signal
TIMEOUT = 1 # seconds
signal.signal(signal.SIGALRM, input)
signal.alarm(TIMEOUT)

if len(sys.argv) != 2:
    print("You must enter an argument.")
    print(len(sys.argv))
else:
    var = sys.argv[1].split("=") 
    try:
        f = float(var[1])
    except ValueError:
        print("You must enter a float.")
        exit()

    far = float(var[1])*1.8 + 32
    print("Celsius: " + var[1] + ". Farenheit: " + "{:.2f}".format(far))