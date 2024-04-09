Alternatives folder 
	- Contains Wifi module 
		- Did not use becaues the power draw was around ~400mA

early-development folder
	contains bluetooth module example that is provided by raspberry pi documentation.
	The folders show the development process of the bluetooth module. An IMU folder
	which contains initial code for being able to read data from the MPU 6050. cellular
	-test folder which contains the HTTP commands to be able to send and receive data 
	from the remote firebase server. 

IMUCalibrationFromCentral
	- This file is the main module that establishes communication between the peripheral raspberry pi pico's to
	the central picos. Majority of the development of this module was done in the branches 	'IMUCalibrationFromCentral' and 'IMUCalibrationFromCentralNoClass'. 
	
	- The important files inside this directory are 'central-left' and 'peripheral-left'. The 'central-right'
	and 'peripheral-right' files are the copies of central-left/peripheral-left with the only difference being
	the UUID that is defined in the bluetooth module. 

sending-receiving-interrupts
	- This directory contains the various methods used to achieve communication between the central-boards and 
	the main raspberry pi pico that is running FreeRTOS. Since SPI communication between pico's is not 
	feasible between pico's running micropython and C as both pico's attempt to drive the CLK line high. The 
	files in this directory were alternatives for that communication.

	-the file /sending-receiving-interrupts/receiving_high_to_low/interrupt-task-reception.c was the file that 
	was eventually used in integration.