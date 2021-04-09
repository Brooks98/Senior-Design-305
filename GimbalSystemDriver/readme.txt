STATUS: DEBUGGING. All basic functionality implemented

Issues we faced and how we solved them: 

        Got it to emulate a mass-spring-damper system, but in the "negative" direction, things are very off. May be because our current sensor is only high-side capable. Idk. 
        Going to hope it's issue with PID controller.
        Potential Solutions:
        To combat this, going to try
          1) Integral Anti Windup - Zero Integrator when error term crosses zero 
              This was instrumental in fixing the issues
          2) Limit Integral Term - 
              This helped too, but the previous technique essentially did the same thing
          3) Gain Scheduling (Positive/Negative Regions, Low Angle, High Angle), based on angle or desired torque
              Didn't try this yet. But likely could definitely increase performance of the PID controller. There were lots of compromises between rise time, overshoot,       stability, etc, so different PID parameters in different regions could give us the best of both worlds. Currently, our system is overdamped to prevent oscillations in the low-current/low voltage regions of the motor where it's behavior becomes nonlinear. We could get snappier response in the high voltage/high current region with a higher integral constant, and leave our current integral constant in the low region the same. If we set our integral constant too high, the system becomes unstable in the low region.
          4) Boost that sample rate to the moon (400-800Hz)
              This was also very helpful in increasing the performance of both the PID controller and mass-spring-damper emulation. 200Hz was ok, but with the filtering we have to do onboard our microcontroller gives a slightly laggy response. Not bad though. Next we tried 800Hz, because why not! We have the horsepower! However, doing so required configuring the current sensor to only send a single 12-bit sample, rather than two averaged together, which provided better onboard filtering, and caused again a laggy response due to a high rise time. 400Hz was the sweet spot, with two averaged 12-bit samples every 25ms.   
          5) Mess With Filtering some more
              IIR filters are the GOAT. Simple calculations, and only need to store a single sample per signal in the filter. Using a low pass IIR filter, we can filter out, or smooth the high frequency content from PWM modulation of the motor drivers. Not an ideal system, but works suprisingly well. We get to retain a high bandwidth compared to human reaction times.
          6) Introduce Dead Zones
              Dead zones were't neeeded, our system is stable after implementing the integral windup and other improvements. Still nice to have it parameterized in though.
          7) Zero Integrator when crossing zero angle    
              I also did this, at the same time as I did step 1. Need to test them invdividually to see which was the real winner.




Using an object oriented approach because it would become nightmare spaghetti without; this will give a super clean interface and keep our main file nice and clean.

Here, I want to build a class that has all the tools we need to fine tune the control of our motors.
Hopefully then, actually building the control algorithms for the haptic feedback will be much simpler.
Included will be motor and gimbal system parameters.

For simple PID control, we don't need these. However, it would be interesting to try a state-space method and utilize these. Since we should have all these parameters, 
"Smoke em if you got em" as they say. We'll use Kv/Kd to convert between voltage/speed and current/torque.

Here's my psuedo-code, which in this case is just a layout of what the object should be able to do. Most is already

The class will interface with the following hardware
1) L298N DC Motor Driver -> DC Brushed Motor
2) INA219 Current Sensor
3) Onboard Hall Effect Encoder
4) Potentiometer (Likely only if we're not using a gearbox, encoder performance was over 100 counts w/ gearbox)

It will have the following configuration/maintenence methods
-Constructor
-Destructor? (Won't need this if no dynamic alloc)
-Set L298N Pins
-Set INA219 Pins
-Set HA Encoder Pins
-Configure Pins
  -Set as outputs or inputs
  -Set PWM frequency 
  -Set PWM resolution
  -Anything else we need
 -Set Current/Torque Control PID Constants
 -Set Speed Control PID Constants
 -Set Motor Stall Threshold
 -Set Motor Dead Zone
 -Set Gain Scheduling
 -Configure Motor Constants
 -Configure Encoder
 -Configure Current Sensor
 And the followingable action user-accessible methods
 -Drive Speed
 -Drive Current
 -Drive Torque
 -Drive Raw
 -Read Current
 -Read Torque
 -Read Speed
 -Read Position
 
 Other Classes we'll need in this class:
 Adafruit_INA219  - Altered, open-source from Adafruit, sensor breakout retailer
 FilterIIR - Quick little class I wrote to provide an interface for filtering a stream of floats
