# Tractive-System-and-Telemetry-Analysis
Formula Student technical exercise featuring a C++ tractive-system state-machine simulation and MATLAB analysis of vehicle telemetry, suspension behaviour and energy consumption.
C++ Tractive System Simulation

Implemented a seven-state model covering startup, contactor conditions, brake input, inverter initialisation, buzzer timing and error handling.

The demonstration includes normal startup, an inverter initialisation failure, and communication timeout checks: a 200 ms message gap is permitted, while a 201 ms gap triggers Error. Component behaviour is simulated in software.
