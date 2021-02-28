# PROJECT
This project develops a wireless sensor network in order to inform fire stations for the environmental conditions in large forest areas.
A beacon (device that contains temperarure, moisture, soil moisture and fire sensors) is placed in a certain area in the forest.
This beacon receives an availability message for each fire station.
In case a fire station is available, the beacon transimts env. conditions every 15 mins, otherwise (fire station unavailability) it stops transmiting information.
For this project we used:
 -2 arduino boards.
 -2 RFM22 tranceivers.
 -1 temperature sensor.
 -1 moisture sensor.
 -1 soil moisture sensor.
 -1 fire sensor.
For the communication between the 2 nodes, we used the ALOHA protocol.
