# PID Temperature Control with Arduino

Ever woken up cold in the winter mornings despite going to bed warm? To solve that, I designed a PID-controlled heater using an Arduino Uno. 
The system lets you select a desired temperature, which it maintains throughout the night, ensuring consistent comfort.

While the heater is a small hotend rather than a full-room heater, the same control logic could be applied to larger systems.

### Technical Highlights

- Used a thermocouple sensor to get a temperature measurement, allowing the Arduino to operate accurately.
- Tuned a PID controller to achieve a rise time of ~100 s, overshoot below 5% (0.5°C), settling time of 3 minutes, and minimal steady-state error (0–0.2°C).
- Added an emergency stop feature to prevent overheating, paying extra attention to safety and risk assessment.
- Gained valuable experience in embedded systems, electronics, sensors, and PID control.

### Demo Video (Google drive) 
https://drive.google.com/file/d/1rdCdCOWQbntFVMdgdJ8dIeNnGaBQTtBa/view?usp=sharing
