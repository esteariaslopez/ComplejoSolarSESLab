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


% This example sets the maximum accuray for DCV. It illustrates how to use NPLC or resolution settings. It takes a single reading.
% We also go through and take multiple readings using a single trigger. We show the relationship to Timeout parameters when taking longer readings.

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
    fopen(myDmm);
    
    % Example to query the instrument.
    fprintf(myDmm, '*IDN?');
    idn = fscanf(myDmm)
    %Configure for DCV 100V range, 10mV resolution this is a fast reading
    fprintf(myDmm,'CONF:VOLT:DC 100, 0.01');
    
    %Now let's set the maximum resolution, there are two methods
    
    %1. Setting the NPLC to 100 will give the DMM the most time to integrate the readings going into it. NPLC = Number of power line cycles.
    % 1 NPLC = 1 power line cycle. Example 1 NPLC in US = 1/60Hz = 16.66 ms. 100NPLC = 1.6 seconds.
    % Using an NPLC setting has the added benefit of rejecting power line noise i.e. normal mode rejection.
    % Setting the maximum NPLC ensures the maximum resolution without needing to calculate or lookup resolution in measurement units i.e. V, I, Ohms
    
    fprintf(myDmm,'VOLT:DC:NPLC 100'); %Set to 100 NPLC, NPLC can be set to 0.02|0.2|1|10|100
    
    % 2. The other way to get maximum resolution is to set the resolution. Setting the resolution is more intuitive to directly set how many
    % digits you want to in your readings. However, this requires a little bit of calculations. For the 34461A, max resolution = 0.3ppm. For
    % 34460A, max resolution = 3.0ppm.
    %
    fprintf(myDmm,'VOLT:DC:RES .0003'); % 34460A Max resolution on 100V range = 100*3e-6 = .0003
    fprintf(myDmm,'VOLT:DC:RES .00003'); % 34461A Max resolution on 100V range = 100*0.3e-6 = .00003
    
    %Autozero: the instrument internally disconnects the input signal following each measurement and takes a zero measurement.
    % It then subtracts the zero measurement from the preceding measurement.
    % This prevents offset voltages present on the instrument's input circuitry from affecting measurement accuracy.
    
    fprintf(myDmm,'VOLT:DC:ZERO:AUTO ON'); %Turn auto zero on for each measurement.
    fprintf(myDmm,'READ?');  %Trigger the DMM to start the readings, can also use INIT and FETCH commands
    DCVResults = fscanf(myDmm)
    
    
    CheckDMMError(myDmm); %Check if the DMM has any errors
catch MExc
    MExc.message
    delete_all_objs()
    
end


