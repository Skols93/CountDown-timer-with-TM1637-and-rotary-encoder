Arduino Countdown Timer with Rotary Encoder and TM1637 Display
This project is an Arduino-based countdown timer that uses a rotary encoder for input and a TM1637 4-digit seven-segment display to show the time. The project allows you to set a countdown time, start the countdown, and reset the timer using a simple interface.

How It Works
1. Setting the Time
When you power up the system, the display will prompt you to set a time for the countdown.
You can use the rotary encoder (a knob that turns and clicks) to adjust the time. Turning the knob changes the digits on the display, and pressing the knob selects the next digit.
The digits represent hours and minutes, allowing you to set the desired countdown time.
2. Choosing an Action
After setting the time, the system offers two options: start the countdown or reset the time.
You can select between these options by turning the rotary encoder and pressing it to confirm your choice.
3. Starting the Countdown
If you choose to start the countdown, the display will show the remaining time, and the countdown will begin.
The time decreases every second, and the display updates accordingly to show the remaining time.
4. Finishing the Countdown
When the countdown reaches zero, the display will show the word "DONE," indicating that the countdown is complete.
5. Resetting the Timer
If you choose to reset the timer, the display will clear, and you can set a new countdown time using the rotary encoder.
Features
Debounce Mechanism: Ensures that the system accurately detects button presses without errors.
Interrupts: The system uses interrupts to manage time-sensitive tasks, making the countdown accurate and responsive.
Components Used
Arduino Uno
TM1637 4-digit seven-segment display
Rotary encoder with push button
Various resistors and connecting wires
License
This project is open-source and available under the MIT License.

