ISSUES: Got it to emulate a mass-spring-damper system, but in the "negative" direction, things are very off. May be because our current sensor is only high-side capable. Idk. 
        Going to hope it's issue with PID controller
        To combat this, going to try
          1) Integral Anti Windup - Zero Integrator when error term crosses zero 
          2) Limit Integral Term - Was broken before
          3) Gain Scheduling (Positive/Negative Regions, Low Angle, High Angle), based on angle or desired torque
          4) Boost that sample rate to the moon (400-800Hz)
          5) Mess With Filtering some more
          6) Introduce Dead Zones
          7) Zero Integrator when crossing zero angle

STATUS: DEBUGGING. All basic functionality implemented

Using an object oriented approach because it would become nightmare spaghetti without; this will give a super clean interface and keep our main file nice and clean.

Here, I want to build a class that has all the tools we need to fine tune the control of our motors.
Hopefully then, actually building the control algorithms for the haptic feedback will be much simpler.
Included will be motor and gimbal system parameters.
For simple PID control, we don't need these. However, it would be interesting to try state-space and utilize these. Since we should have all these parameters, 
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
 Adafruit_INA219  - Altered, Need to check liscense. Changed sample averaging parameter. MUST INCLUDE SOURCE IN PROJECT DIRECTORY!
 FilterIIR - Custom, Chris Johnson
