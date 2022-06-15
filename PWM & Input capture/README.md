# PWM & Input capture using HAL :

In this project we made a simple LED detecting game with PWM and input capture.
There are 4 LEDs with different colors and in different time periods one of the LEDs will turn on. You have to push the button assigned to the LED before the next LED turns on that cause this LED to turn off. if you couldn't press the right button at the right time it will be counted as a miss. number of misses will show on the 7-segment. if you get 3 misses you loose and LCD will show "LOOSER", on the other hand if you continue for 60 seconds you will win and LCD will show "WINNER".

- for entering the game you have to first use the keypad to insert your student number (you only can play this game with correct student number : 97243076 / 97243091). your student number will show every 2s with basic timer.
- after that if you press Reset button for 2 times and each time hold it for 2s the game will be started (input capture).
- LOOSER & WINNER will show every 1s.
- if you press Reset for 3s or press \* in the keypad you will reset the game.
- in first 9s LEDs will turn on every 3s, in next 6s: every 2s, in the next 10s: every 1s, in next 10s: every 900ms and then period for turning LEDs on will reduce 100ms every 10s.
- if you loose, red LED will blink every 500ms (PWM) and if you win green LED will do the same.

(this project is implemented with HAL)

<img src="https://github.com/mrezaamini/Arm-Cortex-M4-ExampleProjects-Using-STM32F401RE/blob/main/PWM%20%26%20Input%20capture/src.png" alt="screenshot" width="500"/>
