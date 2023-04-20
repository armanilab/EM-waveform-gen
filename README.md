# EM-waveform-gen
Arduino-based waveform generator for electromagnets

This project is intended to control high-current electromagnets that cannot be controlled with a standard function generator due to current requirements. 
It features:
  - self-contained integrated user interface and display
  - 3 options for waveforms:
      1. DC voltage
      2. Positive pulse wave
      3. Positive and negative pulse wave
  - user-set frequency

Hardware requirements (all sourced from Sparkfun):
  - Sparkfun RedBoard Qwiic (DEV-15123)
  - Sparkfun 20x4 SerLCD - RGB Backlight (Qwiic) (LCD-16398)
  - Sparkfun Button Pad 2x2 - Breakout PCB (COM-09277)
  - Sparkfun Button Pad 2x2 - LED Compatible (COM-07836)
  - 4x LED - RGB Clear Common Cathode (COM-00105)
  - 4x Diode Small Signal - 1N4148 (COM-08588)
