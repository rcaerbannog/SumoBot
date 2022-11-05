# SumoBot

A small Arduino project for my grade 12 robotics class (TER4M1). The idea behind this is that two sumobots would be placed in a circular ring demarcated by dark tape, and each sumobot would try to find the other using its distance sensor and push it out of the ring. Due to COVID restrictions put back into place in the quadmester I took the course, we were unfortunately unable to build this in person, so we used TinkerCAD to simulate the circuit.

We defined our own search pattern keeping in mind the geometry of the ring (just a circle) and the angle ranges of the distance sensor. We arranged three ultrasonic distance sensors with overlapping vision to create five discrete zones ahead of the robot in which the detected opponent sumobot could be located (Left, Left+Middle, Left+Middle+Right, Middle+Right, Right). 

We were allowed only two pairs of an LED and LDR (photoresistors) to detect the dark tape marking the edge of the ring. We placed one on the front-left and one on the front-right, so that we could tell at what angle the robot approached the ring edge (glancing left, head-on, or glancing right), and determine in which direction the robot should turn to get away from the ring edge.

In this project, I:
* practiced programming logic in Arduino
* learned how to use an ultrasonic distance sensor to detect objects ahead
* learned how to use an L293D half H-bridge IC to control the speed of two motors

The circuit is available here. https://www.tinkercad.com/things/3VDqxxp8A2C

The testing documentation is available in the Submission Document pdf. This describes the expected functionality of each component, and an exact order to test the functioning of the different components, and the correct behaviour of the sumobot when different inputs are read (e.g. LDR detects decrease in light from ring edge, distance sensors detect opponent robot in one of the five zones). 

I also used SketchUp to create a physical model of the SumoBot, of which two images are included.
