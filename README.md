# PD_LineFollowerRobot
A line follower robot with a PD controller. It uses the PIC16F877a coded in baremetal c, TB6612FNG motor driver, and 3 TCRT5000 IR sensors. 

# Hardware
- PIC16F877a
- TCRT5000 Module (3x)
- TB6612FNG motor driver
- DG01D motor (2x)
- 12V to 5V converter module
- 6 in series NiMH batteries (straight to V_motor of motor driver)
- 9V Battery (for logic, regulated by converter module)

# How it works
A switch case assigns error values corresponding to readings. A memory solution was developed to save the previous turn direction whenever it loses the line.
It then uses PD (dt is ignored since the sensor is sampled at a constant rate) to calculate the speed for both motors.

# Demo
[Watch demo on Youtube] https://youtube.com/shorts/KxIhKxUHnCQ?feature=share

# Build
Assembled and programmed using MPLABX IDE v6.30

# Images
<img width="4096" height="3072" alt="IMG_20260507_203714" src="https://github.com/user-attachments/assets/ec9de223-a95f-4432-8f36-36d4c55a412f" />
