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
Lmm  = 1.2E-3;      % Inductance     (H)
Rmm  = 16;          % Resistance     (Ohms)
Kbmm = 1;           % Back EMF       (RPM/V)
Ktmm = 60/(2*pi*Kbmm); % Torque Gain    (Nm/A)
Jmm  = 0;           % Inertia        (kg m^2)
Bmm  = 1;           % Friction       (kg m^2/s)
Mmm  = 108E-3;      % Mass           (kg)
mainBracketMass = 0;

% Main Rotor
Nmr  = 2;           % Number of Blades
Lmr  = 1;           % Length         (cm)
Mmr  = 1;           % Mass           (g)
Fmr  = 1;           % Friction       (gcm^2/s)
TPmr = 1;           % Thrust Axial   (mg/RPM)

% Tail Motor
Ltm  = Lmm;           % Inductance     (H)
Rtm  = Tmm;           % Resistance     (Ohms)
Kttm = Ktmm;           % Torque Gain    (Nm/A)
Kbtm = Kbmm;           % Back EMF       (RPM/V)
Jtm  = Jmm;           % Inertia        (gcm^2)
Btm  = Bmm;           % Friction       (gcm^2/s)
Mtm  = Mmm;           % Mass           (g)
tailBracketMass = 0;

% Tail Rotor
Ntr  = 2;           % Number of Blades
Ltr  = 1;           % Length         (cm)
Mtr  = 1;           % Mass           (g)
Ftr  = 1;           % Friction       (gcm^2/s)
TPtr = 1;           % Thrust Axial   (mg/RPM)

% Drone
Mh  = 1;            % Mass           (g)
B1h = 0;            % Air R          (Ns/cm)
Lh  = 1;            % Length         (cm)
Jh  = 1;            % Mom of Inertia (gcm^2)
B2h = 1;            % Rot Air R      (gcm^2/s)


% Circuits
Vss = 5;            % V