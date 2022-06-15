# Analog to Digital & Digital to Analog Converter (ADC & DAC):

This project contains:

- STM32F401RE microcontroller
- 7-segment mux-2
- 3 push buttons
- 3 LEDs
- DAC_16
- digital oscilloscope
- LM35 (temperature sensor)
- Signal generator

In this project we made a systam that use 2 ADC channels. With one of them it will get a sine signal and the other one is connected to out temperature sensor. If temp is less than 45 degree, system will do sampling from sine signal and show it on the oscilloscope. This system starts when you push start button and will work in different modes:

- Checking : orange LED will blink and we will sample temp every 20ms for 200ms and average is the main temp.
- Ok : if temp less than 35 degree (green LED)
- Warning : if temp is between 35 and 45 degree (orange LED)
- Danger : if temp is higher than 45 degree (red LED blinking) and you have 500ms to press second button (that is responsible for cooling)
- Cooling : second button is pushed and you have 2s to get the temp lower than 35 degree if not, system will turn off and red LED will blink fo 500 ms (third button is responsible for restarting the system)

<img src="https://github.com/mrezaamini/Arm-Cortex-M4-ExampleProjects-Using-STM32F401RE/blob/main/ADC%20%26%20DAC/src.png" alt="screenshot" width="500"/>
