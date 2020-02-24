# Assisted Car

This project is about creating a prototype of a assisted car which is controlled remotely. It comes with the objective of making the  car safer and to stop in the moment of detecting objects in front or behind it. It is a project accomplished by a team of two persons : Mancila Doru-Bogdan si  Balica Adrian-Claudiu.

# Description
There are two objectives to take in consideration . The features and the design.
Firstly , i want to describe it a bit. The project contains two main parts : the car and the  remote controler. The remote controler has a joystick which control the moves of the car, a WI-FI module for the comunication and an arduino nano. The car has an arduino mega which control two dc motors through a L293D chip, a servo motor for the front senzor ( it acts like a radar to detect front collisions), 4 leds( 2 red for behind , and 2 for the signalization ), two sensors ( front and behind) and a buzzer. All of these components are connected through a breadboard.

# What it does?
The main feature of the car is the ability to stop immediately if it detects an object in front or behind it. In front it has a radar ( based on a ultrasonic sensor and a servo motor) which detects the objects in the front side of the car, if any ( within a 60 degrees angle). Also for behind we have  a fixed sensor which takes care of the object behind it to a specific distance. In the moment of detecting an object ( even if he is behind or in front) the car stops to avoid the collision. It doesn't allowed you to move forward or backwards if an object is too close to your car.\
In addition, we added for the design 4 leds and a buzzer. Two of them (yellow) we used for create the  signalization ( the leds blinks when you want to move right or left). The red ones we used for behind when you want to go backwards . They blink on the same time with the sound of the buzzer which simulate a real sound of a real car when it goes backwards.
 


## Bill of materials

The following table represents the materials which were used in creation of the project:

|No                |Components                          |Quantity                         |
|----------------|-------------------------------|-----------------------------|
|1|Arduino Mega|1|
|2|BreadBoard|1|
|3|Buzzer|1|
|4|Red Led|2
|5|Yellow Led|2
|6|Wires| a lot of them
|7|Servo motor|1
|8|DC motor|2
|9|L293D chip|1
|10|220 ohm resistor|4
|11|100 ohm resistor|1
|12|Ultrasonic sensor|2
|13|Ultrasonic sensor support|2
|14|WI-FI modules|2
|15|10 microF capacitor|1
|16|Adaptor WI-FI module|2
|17|Arduino Nano|1
|18|Battery support| 1
|19|Battery|5
|20|Joystick|1



[Images](https://imgur.com/gallery/2eeovBh)
[Demonstration video](https://drive.google.com/open?id=1LJbFb7fC2gjK904bWaTxdY76Ju0Edol7)
