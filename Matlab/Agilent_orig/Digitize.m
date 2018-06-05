%'' """""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
%''  © Agilent Technologies, Inc. 2013
%''
%'' You have a royalty-free right to use, modify, reproduce and distribute
%'' the Sample Application Files (and/or any modified version) in any way
%'' you find useful, provided that you agree that Agilent Technologies has no
%'' warranty,  obligations or liability for any Sample Application Files.
%''
%'' Agilent Technologies provides programming examples for illustration only,
%'' This sample program assumes that you are familiar with the programming
%'' language being demonstrated and the tools used to create and debug
%'' procedures. Agilent Technologies support engineers can help explain the
%'' functionality of Agilent Technologies software components and associated
%'' commands, but they will not modify these samples to provide added
%'' functionality or construct procedures to meet your specific needs.
%'' """""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""


%This program gives an example of how you can use the 34461A to digitize a signal. It follows closely the settings found for the 34411A Digitizing video
%found at http:%www.youtube.com/watch?v=gxLrF4sgAxk The 34461A does not offer all of the triggering options that the 34411A has so some of the features from
%the video are not shown in this programming example.

% First clear any connections with instruments
newobjs = instrfind;
if isempty(newobjs) == 0
    fclose(newobjs);
    delete(newobjs);
    clear newobjs
end
try
    % enter user's instrument connection string
    % DutAddr = 'GPIB0::22''; %String for GPIB
    % DutAddr = 'TCPIP0::169.254.4.61';  %Example string for LAN
    %string DutAddr = 'USB0::0x0957::0x1A07::MY53000101'; %Example string for USB
    %DutAddrr = 'USB0::0x0957::0x1C07::US00000069::0::INSTR';
    DutAddr = 'TCPIP0::156.140.92.16';  %Example string for LAN
    
    %connects with instrument and configures
    myDmm = visa('agilent',DutAddr);
    set(myDmm,'EOSMode','read&write');
    set(myDmm,'EOSCharCode','LF') ;
    set(myDmm,'InputBufferSize', 400000); %Increase the default input buffer size to read lots of readings
    fopen(myDmm);
    
    % Example to query the instrument.
    fprintf(myDmm, '*IDN?');
    idn = fscanf(myDmm)
    
    
    %Configure the 34461 DCV Measurement state
    fprintf(myDmm,'CONF:VOLT:DC 10');              %Configure DCV, 10V range
    fprintf(myDmm,'VOLT:IMP:AUTO OFF');       %10M input impedance setting , 'ON' sets >10Gohm
    fprintf(myDmm,'SENS:VOLT:DC:ZERO:AUTO OFF');   % Autozero Off
    fprintf(myDmm,'VOLT:NPLC .02');               %Set Integration Time;Resolution to .02 PLC
    CheckDMMError(myDmm);                      %Check the DMM for errors
    
    
    %Configure trigger settings
    fprintf(myDmm,'TRIG:SOUR IMM');                %Set trigger source to internal
    fprintf(myDmm,'TRIG:SLOP POS');                %Set trigger slop to pos
    fprintf(myDmm,'TRIG:COUN 1');                  %Set trigger source to internal
    fprintf(myDmm,'TRIG:DEL:AUTO OFF');            %Turn OFF Automatic trigger delay
    fprintf(myDmm,'TRIG:DEL 0');                   % Set trigger delay to 0 sec
    CheckDMMError(myDmm);                      %Check the DMM for errors
    
    %Configure Sample settings
    fprintf(myDmm,'SAMP:COUN 10000');              %Set sample count = 10000
    CheckDMMError(myDmm);                      %Check the DMM for errors
    
    %Trigger and make the readings
    set(myDmm,'timeout',15);  %Allow 12 seconds for DMM to make readings
    fprintf(myDmm,'INIT');                           %Put DMM in wait for Trigger state
    
    fprintf(myDmm,'FETCH?');                        %Get all the readings from Buffer
    
    %Now Populate the console with the readings
    Readings = fscanf(myDmm)                 %Get the readings in a string format
    
catch MExc
    MExc.message
    delete_all_objs()
    
end


