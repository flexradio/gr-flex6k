%clear all;
close all;

% load("ts_debug-7-12_1.mat");

% load("ts_debug-7-12_2Rx.mat");

% load("ts_debug-7-12_3Tx.mat");
% tsdebug=tsdebug(160:end);

load("ts_debug-7-12_6_nxp.mat");

ts_start = tsdebug(1);
ts_end = tsdebug(end);




m = (ts_end-ts_start)/(length(tsdebug)-1);

m = (128/24000)*10^9;

b = ts_start;

x=(0:length(tsdebug)-1)';

y=x*m+b;

zeroed_data = tsdebug-y;

xtime=0:128/24000:(length(tsdebug)-1)*128/24000;

f1 = figure;
plot(zeroed_data)
dcm1 = datacursormode(f1);
set(dcm1, 'UpdateFcn', @NewCallback, 'Enable', 'on');
% plot(xtime,zeroed_data)