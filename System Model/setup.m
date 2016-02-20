% ELEC 391 Project: Control System's Group
% AUTHORS: Wyatt & Aftab

% This script sets all model parameters for the ELEC 391 helicopter control project

clear all;  % Erase everything from memory
constants;  % Assign physical constants

% Example:
% For Dynamics Blocks
% N = [1 2 3];
% D = [4 5 6];
% Means:
% Transfer Function = (s^2 + 2s + 3) / (4s^2 + 5s + 6)
% All Dynamics blocks are initialized to N/D = 1/1 = 1

% ======================
% CONTROLLER & AMPLIFIER
% ======================
% PID Controller
mPID = [1E-20, 0, 0];
tPID = [0,0,0];

% Bias
CurrentDriverBias = Vss/2;

% Current driver gain
% Max voltage for the motor is 12V. The output of the computer is 0-5V where 2.5V
% is 0 velocity. So 2.5/12=4.8
ControlDriverGain = 4.8;

% Min and Max height
% This is determined by physical hardstops
MaxHeight = 0.05; %EXAMPLE VALUES - CHANGE THESE
MinHeight = -0.05; 

% Saturation
mSat = Vss/2;
tSat = Vss/2;

% =====
% MOTOR
% =====
% Elec Dynamics (A/V)
mElecDynNum = [1];
mElecDynDen = [Lmm Rmm];
tElecDynNum = [1];
tElecDynDen = [Ltm Rtm];

% Torque Constant (Nm/A)
mTConst = Ktmm;
tTConst = Kttm;

% Mech Dynamics (RPM/Nm)
% Must compute rotor dynamics from available constants
Jmr = 1/12 * (2*Lmr)^2 * Mmr;
Bmr = Fmr * Nmr;
mMechDynNum = [RPMPerw];
mMechDynDen = [Jmm+Jmr Bmm+Bmr];

Jtr = Jmr;
Btr = Bmr;
tMechDynNum = [RPMPerw];
tMechDynDen = [Jtm+Jtr Btm+Btr];

% Rotor Drag Torque (Nm/RPM)
% (gcm2/s) * (Kgm2/gcm2) / (RPM / rad/s) = (Kgm^2/s^2)/RPM = Nm/RPM
mDrag = Fmr * Kgm2Pergcm2 / RPMPerw * Nmr;

% Back EMF (V/RPM)
mBackEMF = 1/Kbmm;
tBackEMF = 1/Kbtm;
          
% ==========
% HELICOPTER
% ==========
% Gravity (N)
% Include motors, gear, rotors, and heli
%M = (Mmm + Mmg + Mmr*Nmr + Mtm + Mtr*Ntr + Mh) * 1e-3;   % Kg
%Grav = M*G;                   

% Altitude Helicopter Dynamics:
% Heli Lift (Nm/RPM)
mLiftTorque   = TPmr * G * mainArmLength; % Kgm^2/s^2 / RPM  = Nm/RPM
torqueFromYawMotorMass = (tailBracketMass + Mtm) * G * yawArmLength;
torqueFromAltMotorMass = (mainBracketMass + Mmm) * G * mainArmLength;

mHeliDynNum = DegPerRad;
mHeliDynDen = [J1h B1h 0];
altSaturation = 45;

% Yaw Helicopter Dynamics:
% Tail Torque (Nm/RPM)
tTorque = TPtr * G * yawArmLength;  % Kgm/s^2 / RPM * m = Nm/RPM

tHeliDynNum = [0 0 1]; 
tHeliDynDen = [J2h B2h 0];   % Rotational Inertia / Air R
yawSaturation = 180;

% =======
% Sensors
% =======
% Altitude ADC:
ADC_bitwidth = 10;
ground = 0;

% Altitude Potentiometer
angleRange = 145; % Potentiometer can turn +/- 145 degrees
potentiometerGain = Vss/(2*angleRange);
potentiometerBias = Vss/2; % It is mounted so that is outputs 2.5 V
                           % when the angle is 0
% Yaw encoder
yawEncoderQuantization = 360/400; %360 degrees/400 pulses per revolution
