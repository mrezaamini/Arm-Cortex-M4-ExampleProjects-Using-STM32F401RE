# Microcontroller Timer & Counter :

This project contains:

- STM32F401RE microcontroller
- 2x16 character LCD (LM016l)
- 3 push buttons

In this project we made a chronometer with 0.001s accurecy. first it will show "Wellcome" on the LCD and when the first button is pushed it will start counting. With pushing second button counting will be stopped. Then if you push the first button it will be resumed and if you push the third button it will reset to 0. we always show time in this format: MM:SS:mmm.
finally if you press and hold thirsd button for 3 seconds timer will turn off and it will show "Turn off" every 500ms. other buttons and timer itself will not work after this.

(this project is implemented with CMSIS-Core)

<img src="https://github.com/mrezaamini/Arm-Cortex-M4-ExampleProjects-Using-STM32F401RE/blob/main/Microcontroller%20Timer%20%26%20Counter/src.png" alt="screenshot" width="500"/>
