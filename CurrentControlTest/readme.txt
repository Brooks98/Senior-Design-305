Github Tips:

To create directory create a file as /foldername/filename.txt

A readme.txt is automatically displayed when it's directory is pulled up

UPDATE: -It's working pretty well using IIR filtering of the input data. 
        -PWM frequency definitely seems to have an effect on the stability of the current response, or well the noise in the current sensor
        -Using a 200Hz refresh rate on current and PWM data in this. 4 x 12bit current samples for 4.2 something conversion time on the INA.
        -Step response of current is definitely still underdamped, or maybe it's still that pesky noise. But, it's oscillations seem to stay withing +-5-10% of the target value, and seem sinusoidal with a constant frequency. The oscillations do get smaller in steady state but still remain somewhat significant
        -
        
Issues - Sometimes the serial plotter acts up but it only seems to be when I mess with the power supply or touch something
 

Issues: Need to alter Adafruit INA_219 Library. Will do a workaround to get it portable for easier group testing DONE

        May want to go down to a lower sampling averaging value to run system at a higher rate. Aiming for 60fps lol DONE
        Right now each cycle takes around 75ish ms, conservatively, so we're not getting much more than 10fps. DONE
        
        UPDATE: 50Hz is working pretty good. Gonna keep playing with number of samples and rate and such. Still need to try to mess with a MAF. DONE
        
        UPDATE 2: MAF definitely works pretty better. Need to mess around and paramterize everything even more. Maybe try much higher sample rate and bigger MAF. Maybe we can try a higher PWM frequency for the L298N input. DONE
        
        UPDATE 3: Raising system refresh rate helps a lot, we need to mess with PWM frequency to match. Also try exponential MAF, FIR, IIR, etc, and optimize MAF algorithm.
                  Also need to trail off the integral term at somepoint. DONE
        One idea is to lower the averaging rate, then doing a moving average filter of the output data. DONE
        
        Derivative and Integral Error terms need to likely be re-evaluated DONE
        
        UPDATE: They were wrong, simple fix helped enormously. It seems the derivative terms actually helps sometimes? Idk. Should do a linear regression on past so many values for a filtered slope. Or just MAF the derivative term. Hell, MAF everything!
        
        Maybe make a memory object storing last N values and go by that? NO 
        
NOTES
        So Kp is the proportional constant, Kd is the derivative constant, and Ki is the integral constant. I added a Kc term, which is the a overall gain multiplier, to scale the values if needed.
        
        The operating frequency is 1/SAMPLE_PERIOD
        
        Any outputs to the motor driver below MOTOR_THRESHOLD puts the motors in to free spin, or free stop mode.
        
        STALL_THRESHOLD trims off it's value from the 8bit PWM full scale range, so we don't operate in that non-linear region.
        NEED TO DO: Scale output values to completely avoid trying to go under the stall threshold DONE
