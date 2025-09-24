# Rolljam Attack on Automatic Door Lock

## Overview

### Script

#### Receiver.ino

This script handles reading and receiving a signal from the remote key fob if there is not access to a tool like UHR or 433Utils.

1. Checks if CC1101 is connected properly to the device
2. Sets a buffer of 64 bytes to read in a signal with
3. Repeatedly reads in data until a signal is over
4. Prints out the received signal, clears the buffer
5. Repeats


#### Jammer.ino

This script is a high powered radio signal, not a jammer, that interferes with the door lock's ability to receive a signal from the remote key fob.

1. Checks if CC1101 is connected properly to the device
2. Sets the interference level to a degree where signals from the key fob cannot be read properly, but other high powered signals can still get through
3. Continuously transmits 0xFF byte while the script is running


#### Transmitter.ino

This script transmits an inputted signal repeatedly through a CC1101 

1. Checks if CC1101 is connected properly to the device
2. Sends the data 30 times, with a 5 second delay in between sends as insurance against data corruption or loss
