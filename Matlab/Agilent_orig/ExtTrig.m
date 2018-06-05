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


%This example sets the DMM up in the Ext trig mode. This mode waits for a signal to come in on the Ext Trig connector then starts a measurement.
%It will setup a trigger count of 5 triggers. This means that one the DMM is in the wait for trigger state, it will accept 5 trigger signals before returning
%to the idle trigger state.

% This example requires an external signal connected to the 'Ext Trig in' connector. The signal should provide a TTL positive going signal to the DMM.
% If the signal is not provided the program will encounter an exception with an error message 'Unexpected I/O data termination'


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
    
    %Configure for DCV 100V range, 100uV resolution
    %Configure for DCV 100V range, 10mV resolution this is a fast reading
    fprintf(myDmm,'CONF:VOLT:DC');
    
    %Configure the Trigger Count
    fprintf(myDmm,'TRIG:COUN 5'); %Set the trigger count to 5.
    
    %Configure the Trigger source
    fprintf(myDmm,'TRIG:SOUR EXT');
    %Configure the Ext Trigger to look for a positive slope signal.
    fprintf(myDmm,'TRIG:SLOP POS');
    
    %Setup the Voltmeter complete connector to output a positive signal. This connector always puts out a signal when the measurement is complete.
    % This command only determines the polarity of the signal. It does not turn the signal on or off.
    fprintf(myDmm,'OUTP:TRIG:SLOP POS');
    
    fprintf(myDmm,'READ?'); %Set the DMM to wait for trigger state and get readings after ext trigger signal.
    results = fscanf(myDmm)
    %Object[] DCVResult = myDmm.ReadList(IEEEASCIIType.ASCIIType_Any); %Get the readings in an array
    
    
catch MExc
    MExc.message
    delete_all_objs()
    
end


