To create directory create a file as /foldername/filename.txt

Issues: Need to alter Adafruit INA_219 Library. Will do a workaround to get it portable for easier group testing

        May want to go down to a lower sampling averaging value to run system at a higher rate. Aiming for 60fps lol
        Right now each cycle takes around 75ish ms, conservatively, so we're not getting much more than 10fps. 
        
        One idea is to lower the averaging rate, then doing a moving average filter of the output data.
        
        Derivative and Integral Error terms need to likely be re-evaluated
        
        Maybe make a memory object storing last N values and go by that?
        
        
        So Kp is the proportional constant, Kd is the derivative constant, and Ki is the integral constant. I added a Kc term, which is the a overall gain multiplier, to scale the values if needed.
        
        The operating frequency is 1/SAMPLE_PERIOD
        
        Any outputs to the motor driver below THRESHOLD puts the motors in to free spin, or free stop mode.
