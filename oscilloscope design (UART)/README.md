# Oscilloscope :

In this project we made a simple Oscilloscope that consist of 2 parts, display part and sampling part. First we sample data and then send it through UART to display part that shows our data on a GLCD. In our communication protocol we have 2 type of packets that types and detail are described below:
<img src="https://github.com/mrezaamini/Arm-Cortex-M4-ExampleProjects-Using-STM32F401RE/blob/main/Microcontroller%20Timer%20%26%20Counter/src.png" alt="screenshot" width="500"/>

- Channel frame 1 : sending display frame for channel 1
- Channel frame 2 : sending display frame for channel 1
- Set parameter A & B : we have sampled detail as X[n] and we want to show Y[n] that is Y[n] = AX[n] + B (A is a float and B is a signed number)
  <img src="https://github.com/mrezaamini/Arm-Cortex-M4-ExampleProjects-Using-STM32F401RE/blob/main/Microcontroller%20Timer%20%26%20Counter/src.png" alt="screenshot" width="500"/>

In our sampling part: we need to use ADC for sampling and conduct some operations on them to make them ready to send. for getting input from user we have different keys in the keypad:

- 1 : A++
- 2 : A--
- 3 : B++
- 4 : B--
- 5 : time shifting (t++)
- 6 : time shifting (t--)
  (our samoling rate is 100KHz)

<img src="https://github.com/mrezaamini/Arm-Cortex-M4-ExampleProjects-Using-STM32F401RE/blob/main/Microcontroller%20Timer%20%26%20Counter/src.png" alt="screenshot" width="500"/>

In our display part: we have 2 buffers for our channels (one for last frame and one for this frame) and we will display recieved data on GLCD.

<img src="https://github.com/mrezaamini/Arm-Cortex-M4-ExampleProjects-Using-STM32F401RE/blob/main/Microcontroller%20Timer%20%26%20Counter/src.png" alt="screenshot" width="500"/>
