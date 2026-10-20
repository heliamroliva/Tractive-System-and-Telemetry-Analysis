# Q1 — Tractive-system state-machine simulation

## Environment and files

- Language: C++17. Verified with MSYS2 UCRT64 g++ 16.2.0 on Windows; any conforming C++17 compiler should suffice.
- Dependencies: C++ standard library only; no external libraries, toolboxes, input files, or hardware.
- `src/WR_TS.cpp` is the main version corresponding to the report listing. `src/WR_TS_1.cpp` is a later existing variant, retained as a separate unchanged source copy. Each is a standalone translation unit with `main()`. No `.h` file existed in the available Q1 sources.

## Build and run

From this Q1 directory:

```powershell
g++ -std=c++17 -Wall -Wextra -Wpedantic src/WR_TS.cpp -o WR_TS.exe
.\WR_TS.exe
```

For the later variant, replace `WR_TS.cpp` with `WR_TS_1.cpp` and choose a different output name. Do not compile the two files into one executable because both define `main()`.

## Behaviour and expected output

The simulation begins in `Idle`. With the shutdown circuit closed, TS Start closes the contactors; an `update(0)` step reaches `WaitR2D`. Applied brakes and Start initialize the inverter. A successful initialization starts the 1,000 ms buzzer and then reaches `Driving` and turns on the simulated R2D LED. Entry actions are console messages, not physical outputs.

The demonstration then checks the 200 ms inverter-message boundary: 200 ms without a message remains `Driving`; 201 ms enters a latched `Error` and stops the inverter. A second independent TS object injects initialization fault code 7 and also reaches `Error`. Opening the shutdown circuit after `Error` does not reset it. Inputs and virtual time are simulated; this is not a hardware controller.

The exact observed stdout from the verified main build is `results/console_output.txt`. `results/console_output_variant.txt` captures the later variant; both outputs matched line by line. Test executables are intentionally omitted from the ZIP; build them from the supplied sources.
