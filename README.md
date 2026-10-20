# Q2 — One-lap vehicle telemetry analysis

## Environment and provenance

- Verified environment: MATLAB R2026b (26.2.0.3386108) on Windows.
- MATLAB toolboxes: none beyond base MATLAB are used by the script or export helper. Functions include `readtable`, `gradient`, `polyfit`, `trapz`, `cumtrapz`, and `exportgraphics`.
- Input: `src/endurance_lap(in).csv`, copied unchanged from the supplied project folder. The file has 1,281 samples and columns for time, distance, GPS and wheel speeds, throttle, brake, four suspension travels, and battery power.
- Analysis source: `src/WR_Q2_analysis.m` was reconstructed line by line from Appendix B of the supplied report because no separate `.m` or `.mlx` file was located. `src/export_results.m` is a new portfolio helper. The appendix states that its Q2(b) plot was added to original calculations; the history of that change cannot be independently verified.

## Run

With MATLAB available, run from any folder:

```matlab
run('C:/c_project/wr_portfolio/Q2/src/export_results.m')
```

For a relocated package, replace the absolute path. The helper changes to `src/`, runs the recovered analysis, and writes outputs to `results/`. To run only the recovered analysis, make `src/` the current MATLAB folder and run `WR_Q2_analysis`.

## Expected outputs and interpretation

- `results/speed_pedals.png`: GPS speed, throttle, and brake against distance. Approximately 14 corner-related slowdowns were assessed visually in the submitted report; this is not a track-map corner count.
- `results/wheel_speed_comparison.png`: wheel-speed channels and rear-right versus GPS speed excess. Candidate intervals are selected by rear-right speed exceeding the **front-wheel mean**, while their printed maximum ratio uses **GPS speed** as denominator. These are different references, so an interval can have a negative printed ratio. Such intervals are exploratory, not confirmed powered wheelspin.
- `results/suspension_travel.png`: four raw suspension channels. The front and rear axle-mean fits against estimated longitudinal acceleration are descriptive; sensor zeroes and dynamics are not calibrated.
- `results/battery_energy.png`: discharge-positive battery power and integrated net energy. The code assumes raw negative battery power means discharge; confirm this against logger documentation before engineering use.
- `results/metrics.json` and `results/matlab_console.txt`: actual numerical output and full printed interval list from the verified MATLAB run.

Observed: lap length 0.8567 km over 64.0 s; net use 0.2168 kWh; recovered energy 0.0246 kWh; linearly extrapolated net energy for 22 km 5.57 kWh. The extrapolation assumes the recorded lap's pace and conditions repeat and excludes any battery reserve. GPS delay, wheel-radius calibration, braking, cornering, and suspension sensor calibration limit physical interpretation. The CSV's public redistribution rights are unconfirmed; keep this as a private review package.
