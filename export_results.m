% Portfolio export helper. Runs the report-appendix Q2 analysis unchanged.
script_dir = fileparts(mfilename('fullpath'));
cd(script_dir);
run('WR_Q2_analysis.m');

% The original script calls clear, so recover this path after it returns.
script_dir = fileparts(mfilename('fullpath'));
results_dir = fullfile(script_dir, '..', 'results');
if ~exist(results_dir, 'dir'), mkdir(results_dir); end
figs = findall(groot, 'Type', 'figure');
for k = 1:numel(figs)
    switch figs(k).Number
        case 1
            filename = 'speed_pedals.png';
        case 2
            filename = 'wheel_speed_comparison.png';
        case 3
            filename = 'suspension_travel.png';
        otherwise
            filename = sprintf('figure_%d.png', figs(k).Number);
    end
    exportgraphics(figs(k), fullfile(results_dir, filename), 'Resolution', 160);
end

energy_fig = figure('Name', 'Q2(d) Battery energy', 'Color', 'w');
subplot(2,1,1);
plot(distance_m, P/1000, 'LineWidth', 1);
yline(0, ':'); grid on;
ylabel('Battery power (kW)');
title('Discharge positive; regeneration negative');
subplot(2,1,2);
plot(distance_m, cumtrapz(time_s, P)/3.6e6, 'LineWidth', 1);
grid on; xlabel('Distance (m)'); ylabel('Net energy (kWh)');
exportgraphics(energy_fig, fullfile(results_dir, 'battery_energy.png'), 'Resolution', 160);

metrics = struct('matlab_version', version, ...
    'samples', numel(time_s), 'duration_s', time_s(end)-time_s(1), ...
    'lap_length_km', lap_length_km, ...
    'net_energy_kWh', E_net_kWh, ...
    'recovered_energy_kWh', E_recovered_kWh, ...
    'estimated_net_22km_kWh', E_22km_kWh, ...
    'front_fit_slope', front_fit(1), 'front_fit_intercept_mm', front_fit(2), ...
    'rear_fit_slope', rear_fit(1), 'rear_fit_intercept_mm', rear_fit(2));
fid = fopen(fullfile(results_dir, 'metrics.json'), 'w');
fprintf(fid, '%s\n', jsonencode(metrics, 'PrettyPrint', true));
fclose(fid);
fprintf('\nExported four figures and metrics.json to %s\n', results_dir);
