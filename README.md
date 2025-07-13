# Focus-Sense
➤ Built a smart Pomodoro study timer using Arduino Nano and IoT sensors to manage focused study sessions with inactivity detection.


| Button                  | Behavior                                              |
| ----------------------- | ----------------------------------------------------- |
| **Start**               | Starts Pomodoro (25 mins)                             |
| **Pause**               | Toggles between pause and resume                      |
| **Stop**                | Fully stops and resets to Pomodoro                    |
| **Reset Once**          | Resets to 25-min Pomodoro & starts immediately        |
| **Reset Twice Quickly** | Resets to 5-min break & starts immediately            |
| **Auto**                | After Pomodoro ends, starts 5-min break automatically |


**✅ Functionality**

Monitors PIR sensor.
If no motion for 5 minutes, buzz once and set a warning flag.
Then checks again for 1 more minute.
If still no motion, buzz continuously (danger mode).
If motion occurs at any time, reset everything.


**PIR: Sit still for 5 minutes → brief buzz + vibrate → continue still for 1 more minute → continuous buzz + timer pauses.
Move slightly: PIR detects motion → resets everything**

