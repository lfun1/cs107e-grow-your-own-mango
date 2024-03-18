# winter24-project-eliaschikwanda-lfun1

## Project Name: “Grow Your Own Mango” - Weather Station and Drip Irrigation System

## Project Overview
For our CS107E final project, we wanted to interact with the real-world and build a practical tool that can help gardeners. We built a weather station that can sense temperature, humidity, pressure, and wind speed using the BME280 sensor and an anemometer. We also designed and built a drip irrigation system that tracks soil moisture using a soil moisture sensor and waters plants using a solenoid valve depending on the soil moisture level. Finally, we integrated all the sensor data and displayed it on a dashboard with graphs and data values.

## What we accomplished
<ol>
  <li>
    Wrote and compiled drivers to read in sensor data and control solenoid valve
    <ol>
    <li> BME280 sensor: Converted Adafruit's BME280 Sensor Library written in C++ to C code that can run on MangoPi and work with Yifan Yang's SPI module. </li>
    <li> Anemometer: Wrote driver to read in digital Hall Effect sensor signals from anemometer and convert it to wind speed, based on Julie Zelinski's Hall effect sensor demo code. </li>
    <li> Soil moisture sensor: Wrote driver to read in soil moisture sensor values using Julie Zelinski's MCP3008 demo code. </li>
    <li> Solenoid valve: Wrote code to control opening and closing of solenoid valve depending on soil moisture value. </li>
    </ol>
  </li>
  <li>
    Wrote dashboard module layered on graphics library to convert data read in from sensors and display the data on 4 graphs and 2 data panels.
  </li>
  <li>
    CAD and 3D print components for anemometer and drip irrigation system
  </li>
</ol>

