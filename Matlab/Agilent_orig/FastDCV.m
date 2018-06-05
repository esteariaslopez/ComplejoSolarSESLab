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


%In this example program we try to configure the DMM for the fastest DCV readings. To do this, we turn off autorange, null, and auto zero settings.
% We also set the the DMM to its fastest reading rate, examples using NPLC or resolution are shown.
% Finally, we optimize the throughput from DMM to PC rate

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
    
    %Configure for DCV 1V range, 1e-4V resolution for fastest reading on 34461A
    %Setting the range will turn off autorange. Autorange takes a quick reading to determine the range for the DMM. Turning it off will speed up
    %the measurement to you
    fprintf(myDmm,'CONF:VOLT:DC 1, 1e-4');
    
    %You can also set for minimum NPLC for fastest reading
    
    %1. Setting the NPLC to 100 will give the DMM the most time to integrate the readings going into it. NPLC = Number of power line cycles.
    %1 NPLC = 1 power line cycle. Example 1 NPLC in US = 1/60Hz = 16.66 ms. 100NPLC = 1.6 seconds.
    %Using an NPLC setting has the added benefit of rejecting power line noise i.e. normal mode rejection.
    %Setting the minimum NPLC ensures the minimum resolution without needing to calculate or lookup resolution in measurement units i.e. V, I, Ohms
    
    fprintf(myDmm,'VOLT:DC:NPLC 0.02'); %Set to 100 NPLC, NPLC can be set to 0.02|0.2|1|10|100
    SglRdgTime = 0.02 .* (1 / 60); %Calculate the single reading time assuming using a 60Hz power line frequency
    SglRdgTimems = SglRdgTime .* 1000; %Convert the time to milliseconds and to an integer
    
    %Turn off AutoZero so that the DMM is not doing any internal correction reading which will slow down the measurement given to you
    fprintf(myDmm,'VOLT:DC:ZERO:AUTO OFF'); %There is a similar command for most DC functions including Temperature, Current, Resistance
    
    %Turn off Null
    fprintf(myDmm,'VOLT:DC:NULL:STAT OFF'); %There is a similar command for most functions including Temperature, Current, Resistance
    
    %The above commands will configure the fastest readings for the DMM. We can also optimize the data transfer rates to the PC by taking our set of readings
    %then transfer after the end of the readings.
    
    
    % Here we will set the DMM to take multiple readings*/
    %Setup to take 100 readings
    fprintf(myDmm,'SAMP:COUN 100'); %Take 100 readings per sample
    set(myDmm,'timeout',100 .* SglRdgTimems + 500); %Calculate the timeout for 100 readings, add in a small buffer.
    
    
    
    %fprintf(myDmm,'READ?'); %Trigger the DMM to start taking the readings. Turns out that READ? has a little bit of overhead.
    %We can optimize that overhead by using a bus trigger and remove individual readings.
    
    fprintf(myDmm,'TRIG:SOUR BUS'); %Set the trigger system to bus trigger
    fprintf(myDmm,'INIT'); %Get the DMM into Wait for trigger state
    fprintf(myDmm,'*TRG'); %Trigger the DMM to make measurements
    fprintf(myDmm,'DATA:REM? 100, WAIT'); %Get the readings and wait until 100 reading are available in the reading buffer
    DCVResult = fscanf(myDmm)
    
    CheckDMMError(myDmm); %Check if the DMM has any errors
    
    
    
catch MExc
    MExc.message
    delete_all_objs()
    
end


