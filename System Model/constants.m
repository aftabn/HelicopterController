% ELEC 391 Project: Control System's Group
% AUTHORS: Wyatt & Aftab

% This script contains physical constants and specifications

% Physical Constants
G = 9.81;               % m/s^2
FtPerM = 3.28;          % ft/m
RadPerDeg = pi/180;     % rad/deg
DegPerRad = 180/pi;     % deg/rad
Kgm2Pergcm2 = 1e-7;     % Kgm^2/gcm^2
RPMPerw = 30/pi;        % RPM / rad/s

% Desired Trajectory
% Row 1 = value
% Row 2 = time
Alt = [ 00  03  03  18  18  05  00  00
        00  01  05  08  10  15  20  30];
Yaw = [ 00  00  60  60 180 180
        00  05  06  10  15  30];

% Mechanical Properties
% The length of the arm going to the main motor (m).
% Measured from the fulcrum to the end
mainArmLength = 0.15;
% The length of the arm going to the yaw motor (m)
% Measured from the fulcrum to the end
yawArmLength = 0.15;

% Main Motor
Lmm  = 1.2E-3;         % Inductance     (H)
Rmm  = 16;             % Resistance     (Ohms)
Kbmm = 586;            % Back EMF       (RPM/V)
Ktmm = 60/(2*pi*Kbmm); % Torque Gain    (Nm/A)
Jmm  = 0;              % Inertia        (kg m^2)
Bmm  = Ktmm/(Kbmm*Rmm)*RPMPerw;              % Friction       (kg m^2/s)
Mmm  = 170E-3;         % Mass (with bracket)    (kg)
mainBracketMass = 0;

% Main Rotor
Nmr  = 2;           % Number of Blades
Lmr  = 0.133;       % Length of a single blade (m)
Mmr  = 5E-3;        % Mass (total)   (kg)
Fmr  = 0;           % Friction       (kgm^2/s)
TPmr = 2.9E-6;      % Thrust Axial   (kg/RPM)

% Tail Motor
Ltm  = Lmm;           % Inductance     (H)
Rtm  = Rmm;           % Resistance     (Ohms)
Kttm = Ktmm;           % Torque Gain    (Nm/A)
Kbtm = Kbmm;           % Back EMF       (RPM/V)
Jtm  = Jmm;           % Inertia        (kgm^2)
Btm  = Bmm;           % Friction       (kgm^2/s)
Mtm  = Mmm;           % Mass           (kg)
tailBracketMass = (6 + 2*22)*10^-3;

% Tail Rotor
Ntr  = 2;           % Number of Blades
Ltr  = Lmr;           % Length         (m)
Mtr  = Mmr;           % Mass           (kg)
Ftr  = Fmr;           % Friction       (kgm^2/s)
TPtr = TPmr;           % Thrust Axial   (kg/RPM)

% Drone
Mh  = 1;            % Mass           (kg)
B1h = 0;            % Altitude Rotational Resistance (kgm^2/s)
Lh  = 1;            % Length         (m)
J1h  = 0.002;           % Altitude Mom of Inertia (kgm^2)
J2h  = 0.009;           % Yaw Mom of Inertia (kgm^2)
B2h = 0;            % Yaw Rotational resistance      (kgm^2/s)


% Circuits
Vss = 5;            % V