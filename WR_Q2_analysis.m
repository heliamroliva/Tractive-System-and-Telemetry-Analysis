% Warwick Racing Q2 - Xieyi Fan
% Original calculations and event output retained; Q2(b) plot added.
clear; clc; close all;
% Place the supplied CSV beside this script.
data = readtable('endurance_lap(in).csv');
time_s = data.time_s;
distance_m = data.distance_m;
v_gps = data.gps_speed_kmh;
v_fl = data.wheel_speed_fl_kmh;
v_fr = data.wheel_speed_fr_kmh;
v_rr = data.wheel_speed_rr_kmh;
throttle = data.throttle;
brake = data.brake;
z_fl = data.sus_fl_mm;
z_fr = data.sus_fr_mm;
z_rl = data.sus_rl_mm;
z_rr = data.sus_rr_mm;
battery_power_W = data.battery_power_W;
v_front = (v_fl + v_fr) / 2;
v_rear = v_rr;
v_front_ms = v_front / 3.6;
% Acceleration in m/s^2, estimated from non-driven front-wheel mean.
a_x = gradient(v_front_ms, time_s);
delta_v = v_rear - v_gps;
K = nan(size(v_gps));
valid_speed = v_gps > 0; % Avoid division by zero.
K(valid_speed) = delta_v(valid_speed) ./ v_gps(valid_speed);
z_front = (z_fl + z_fr) / 2;
z_rear = (z_rl + z_rr) / 2;
dz_front = z_fl - z_fr;
dz_rear = z_rl - z_rr;
% Corner count is assessed visually from speed and pedal traces.
corner_distance_m = [];
spin_ranges_m = [];
figA = figure('Name','Q2(a) Speed and pedals','Color','w');
subplot(3,1,1);
plot(distance_m, v_gps); grid on;
ylabel('Speed (km/h)'); title('Q2(a): GPS speed, throttle and brake');
subplot(3,1,2);
plot(distance_m, throttle); grid on;
ylabel('Throttle (0-1000)');
subplot(3,1,3);
plot(distance_m, brake); grid on;
ylabel('Brake (0-1000)'); xlabel('Distance (m)');
% Original candidate detector; this is not proof of powered wheelspin.
spin = v_rear > v_front;
change = diff([false; spin; false]);

start_idx = find(change == 1);
end_idx = find(change == -1) - 1;
for i = 1:length(start_idx)
    first = start_idx(i);
    last = end_idx(i);
    fprintf('Position: %.1f - %.1f m\n', ...
        distance_m(first), distance_m(last));
    fprintf('Maximum slip ratio: %.1f %%\n\n', ...
        max(K(first:last)) * 100);
end
% Q2(b): added comparison plot; calculations and printed events are unchanged.
figure('Name','Q2(b) Wheel-speed comparison','Color','w');
subplot(2,1,1);
plot(distance_m, [v_fl v_fr v_rr v_gps]); grid on;
legend('Front left','Front right','Rear right','GPS');
ylabel('Speed (km/h)');
subplot(2,1,2);
plot(distance_m, K*100); grid on;
ylabel('RR vs GPS excess (%)'); xlabel('Distance (m)');
% Q2(c): raw suspension travel; positive values denote compression.
figure;
plot(distance_m, [z_fl z_fr z_rl z_rr]);
xlabel('Distance (m)');
ylabel('Suspension travel (mm)');
legend('FL', 'FR', 'RL', 'RR');
grid on;
% Descriptive fits; not calibrated spring or load models.
front_fit = polyfit(a_x, z_front, 1);
rear_fit = polyfit(a_x, z_rear, 1);
fprintf('z_front = %.3f * a_x + %.3f\n', ...
    front_fit(1), front_fit(2));
fprintf('z_rear = %.3f * a_x + %.3f\n', ...
    rear_fit(1), rear_fit(2));
% Infer discharge-negative raw power; P is discharge-positive.
P = -battery_power_W;
% W*s / 3.6e6 gives kWh.
E_net_kWh = trapz(time_s, P) / 3.6e6;
P_regen = max(-P, 0);
E_recovered_kWh = trapz(time_s, P_regen) / 3.6e6;
lap_length_km = (distance_m(end) - distance_m(1)) / 1000;
E_22km_kWh = E_net_kWh * 22 / lap_length_km;
fprintf('\nQ2(d):\n');
fprintf('Net energy used on this lap: %.4f kWh\n', E_net_kWh);
fprintf('Energy recovered through regenerative braking: %.4f kWh\n', ...
    E_recovered_kWh);
fprintf('Estimated net energy required for 22 km: %.2f kWh\n', ...
    E_22km_kWh);
