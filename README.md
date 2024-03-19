# winter24-project-eliaschikwanda-lfun1

## Project: “Grow Your Own Mango” — Weather Station and Drip Irrigation System
**By Lisa Fung and Elias Chikwanda**

## Project Overview
For our CS107E final project, we wanted to interact with the real-world and build a practical tool that can help gardeners. We built a weather station that can sense temperature, humidity, pressure, and wind speed using the BME280 sensor and an anemometer. We also designed and built a drip irrigation system that tracks soil moisture using a soil moisture sensor and waters plants using a solenoid valve depending on the soil moisture level. Finally, we integrated all the sensor data and displayed it on a dashboard with graphs and data values.

## Accomplished
1. Wrote and adapted **drivers to read in sensor data and control solenoid valve**
    - **BME280 sensor:** We worked together to convert Adafruit's BME280 Sensor Library written in C++ to C code that can run on MangoPi and work with Yifan Yang's SPI module.
    - **Anemometer:** Lisa wrote a driver to read in digital Hall Effect sensor signals from anemometer and convert it to wind speed, based on Julie Zelinski's Hall effect sensor demo code.
    - **Soil moisture sensor:** Lisa wrote driver to read in soil moisture sensor values using Julie Zelinski's MCP3008 demo code.
    - **Solenoid valve:** Lisa wrote code to control opening and closing of solenoid valve depending on soil moisture value.
2. **Dashboard module:** Lisa outlined and Elias wrote most of the dashboard module layered on graphics library to convert data read in from sensors and display the data on 4 graphs and 2 data panels.
3. CAD, 3D print, and assemble components for **anemometer and drip irrigation system**
    - **Anemometer:** We 3D printed anemometer design created by jostak on Thingiverse. We drilled holes to fit a magnet into the anemometer cap and the Hall effect sensor through the base. We inserted a bearing to the top of the anemometer to achieve smooth spinning.
    - **Drip Irrigation System:** Lisa used CAD to design and 3D print the drip irrigation spout. We worked together to create a circuit to control the solenoid valve using an external 12V power supply, receiving guidance from Jeff Stribling (Lab64). Then, we hot glued the components together and used 1/4'' outer diameter tubing to connect the solenoid valve to the water supply inlet and outlet.

## Reflection
Over the course of our project, we spent long hours reading through data sheets, debugging code/wiring/sensors, and coming up with creative ways to see our weather station and drip irrigation system through. We spent the first week outlining the dashboard module, trying to configure the SPI hardware on the Mango Pi, setting up the Hall effect sensor, 3D printing the anemometer, and looking into the solenoid valve and other sensors. After making some progress on the SPI module, we received a working SPI driver from Yifan Yang and Julie Zelinski (thank you!). From there we focused our efforts on adapting Adafruit's BME280 library and configuring the soil moisture sensors over the weekend. In the second week, we 3D printed and assembled the anemometer with the magnet and hall sensor, implemented the dashboard module for displaying data and graphs, and wired up the solenoid valve with an external 12V power source to control water flow. Finally, we tested the individual components, figured out how to support two SPI communication channels using transistors, and updated the dashboard to accept real-time values from the sensors and update the graphs automatically. It was very rewarding to bring everything we had written and built together, and we appreciate all the support and guidance we received!

### Learned
- Serial Peripheral Interface (SPI) protocol, MCP3008 Analog-to-Digital converter communication, BME280 SPI protocol, and lots of datasheet reading
- How to simple design interfaces for sensor drivers and integrate them into dashboard module
- How to create graphs, store data, and update values to display on monitor via dashboard module
- Computer-aided design (CAD) and 3D printing to design and assemble drip irrigation spout
- How to properly wire solenoid valve with 12V external power and use transistors as switches to support two SPI devices

### Challenges
- SPI hardware configuration on Mango Pi, especially translating the D1-H manual to configuring and setting the correct registers in the proper order.
    - Yifan Yang and Julie Zelinski's SPI module came in at the right time and allowed us to move forward with the sensors!
- Assembling anemometer and drilling holes into 3D printed PLA materials to allow hall sensor to consistently detect magnet.
    - Matt Vaska provided the tools and guidance to allow us to successfully drill the holes after a few failed attempts!
- Integrating external power supply for solenoid valve with lower voltages from Mango Pi
    - Meimei Liu helped us debug and suggested that we connect the solenoid valve 12V power supply to a common ground (GND) with the Mango Pi
- Finding materials! We scrambled to order 1/4'' outer diameter vinyl tubing in time and had to creatively find ways to demo our drip irrigation system with a plastic water bottle and cups of soil
    - We found the hall effect sensor and 12V DC power supply cable at Lab64 and ordered the rest from Amazon.

## Materials
1. BME280 Sensor (Temperature, Humidity, Pressure) (from Julie)
2. Hall Effect Sensor (from Lab64)
3. MCP3008 Analog-to-Digital converter (ADC) (from Julie)
4. [Soil Moisture Sensor](https://www.amazon.com/Capacitive-Moisture-Corrosion-Resistant-Detection/dp/B07SYBSHGX/)
5. [12V Solenoid valve](https://www.amazon.com/dp/B016MP1HX0/?th=1) with 1/4'' inlet, normally closed for drip irrigation
7. [608ZZ Ball Bearing](https://www.amazon.com/Tonmp-PCS-608-ZZ-Ball-Bearing/dp/B0B4JH1XVP/)
8. [Neodymium Disc Magnets](https://www.amazon.com/VSKIZ-Magnets-Assorted-Neodymium-Refrigerator/dp/B0BZYQBFQ1/) (5mm diameter, 2mm thick)
9. CAD and 3D printing at Lab64 for anemometer and drip irrigation spout
10. 1K resistors, 2N3904 transistors (BJTs), diode, MOSFET, capacitor (from CS107E kit, Lab64, Meimei Liu)
11. [Clear Vinyl Tubing (1/4 inch OD)](https://www.amazon.com/gp/product/B07Q1D14HF/)
12. Plastic bottle, cups
13. DHT11 Temperature & Humidity Sensor (for testing only)

## Resources
### Help received
- Julie Zelenski
    - Sensors Lecture code for SPI (bit bang version), MCP3008 module, Hall effect sensor
    - Suggestion to adapt Adafruit BME280 Sensor Library from C++ into C to work on Mango Pi
    - Working SPI hardware module
    - Lots of support and encouragement!
- Yifan Yang
    - Provided Hardware SPI driver on Mango Pi
- Matt Vaska
    - Provided mechanical tools (drill, drill bits, drill bit collar, hot glue gun, calipers) and guidance for drilling holes into anemometer parts
- Jeff Stribling
    - Provided diode and MOSFET for circuit to open solenoid valve
    - Walked us through how to wire up circuit to control solenoid valve with diode to prevent sharp changes in voltage
- Nazar Khan
    - Provided adapter from 12V power supply to breakout VCC and GND, multimeter
- Meimei Liu
    - Helped us fix always-on solenoid valve by suggesting that we connect 12V power supply and Mango Pi to common ground
    - Provided capacitor to protect against sharp changes in voltage
- Aditi Bhaskar
    - Helped Lisa identify incorrect wiring of Hall effect sensor from breakout board

### Sources Used
- [jostak's anemometer design on Thingiverse](https://www.thingiverse.com/thing:2559929)
- [tosa's plant irrigation ring](https://www.thingiverse.com/thing:6491638) (used as inspiration)
- [Julie's Sensors Lecture code](https://github.com/cs107e/cs107e.github.io/blob/master/lectures/Sensors/code/)
- 

## Future Improvements
- File system to store data read
- More robust irrigation system with 3D printed components to hold solenoid valve, water supply in place

## Folder Organization
