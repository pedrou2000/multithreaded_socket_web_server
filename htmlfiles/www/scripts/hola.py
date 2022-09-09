import sys
import signal
TIMEOUT = 1 # seconds
signal.signal(signal.SIGALRM, input)
signal.alarm(TIMEOUT)

var = sys.argv[1].split("=")

if var[1] == "":
    print("You must enter your name first.")
    exit()


print("Hola " + var[1] + "!")