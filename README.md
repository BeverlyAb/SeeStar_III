# SeeStar III
**Summer Project from Monterey Bay Aquarium Research Institute (2017)**

MBARI's **SeeStar III** is a continuation of **SeeStar II**: https://bitbucket.org/mbari/seestar/wiki/Operations%20Manual

This project focuses on the serial communication and power management for **SeeStar III's** "Dictator behavior" on 
**Adafruit's Adalogger Feather M0.** Essentially, the M0 receives commands from SeeStar (Sleepy Pi), such as 
taking samples or returning the last log on file before it enters its default sleep state. This protocol is 
to establish communication between processors and peripheral sensors, as well as reduce the power consumption of the 
SeeStar system and prolong its operating time during long term deployment periods. 

**Serial_Demo** shows examples of serial instantiations and pin muxings on the M0 by daisy chaining
5 UART ports.

**myWakeUpv9** demonstrates a power management procedure, where once signaled to wake up, the M0 executes commands to either read and log sensor data on file (Take Sample), return the last log on file (Return Last), or return the time on the M0 and go back to sleep. The command input are read as strings rather than individual bytes, which hinders how strings representing TIME should be interpreted as integers. (In Progress)

**Serial_Sleep_Protocol** continues from myWakeUpv9, but processes inputs as bytes and stores them into instantiated buffers. This involved incorporating code from Matt Ducasse's senior project with SeeStar III. This only includes Take Sample and Read Last. Integrating code is still in progress and is inherently messy. (In Progress)  


