# USART/UART serial communication :

In this directory we have 2 projects:

- Thermometer : we made a thermometer that consist of a STM32F401RE, a virtual terminal and a temp sensor. We get temp from the sensor using ADC and showing it in virtual terminal using UART port. (we implemented this part only using CMSIS-Core)
  <img src="https://github.com/mrezaamini/Arm-Cortex-M4-ExampleProjects-Using-STM32F401RE/blob/main/PWM%20%26%20Input%20capture/src.png" alt="screenshot" width="500"/>

- Calculator : this project is a calculator that gets numbers and operations from a 4x4 keypad and then send it through UART to itself (with interrupt) and then process recieved data to show the result on LCD.
  <img src="https://github.com/mrezaamini/Arm-Cortex-M4-ExampleProjects-Using-STM32F401RE/blob/main/PWM%20%26%20Input%20capture/src.png" alt="screenshot" width="500"/>
