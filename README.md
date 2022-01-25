# Chunithm-Controller
This is a customed controller for SAGA's arcade game "Chunithm".

Game Official Link: https://chunithm.sega.com/

My Controller Demo Video: https://www.bilibili.com/video/BV13k4y1k7Bm

### Game Introduction
  Chunithm is a classical rhythm game with two types of inputs. 
  Users need to touch the screen and raise their hands at the right time to pass the game. 
  The official machine uses a 32-channel touch screen and 8 pairs of infrared detectors to track users' actions.

### Hardware
  My controller also implemented a capacitive touch screen and 8 pairs of infrared detectors to perform the same function.
  The main control chip is ATmega32U4, a powerful and popular chip for individual embedded development.
  It was connected to four capacitance sensor chips. Each of them can accept 4 channels' signals.
  
  In order to save the production cost and make manufacturing more manageable, I also designed PCBs and outer shells. 
  The CAD files are included in "PCB" and "CAD Blueprints"
  
### Software
  This project utilizes the I/O control library provided by Arduino and an open-source program to process serial communication between the control board and game.
  
### Details
  The detailed information is included in "Report.pdf".
