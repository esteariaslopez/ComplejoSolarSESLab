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



%Uses the CALC subsystem. This system does internal calculations on the data set include statistics and limit checking.
%1)Take 20 readings and turn on internal limits. Set limit and check the questions status byte if a reading was outside of limits.
%2)Then perform math functions including mean, max, min, standard deviation, and peak to peak. The max and min reading will show the readings that were outside of the limits

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
    DutAddr = 'USB0::0x0957::0x1C07::US00000069::0::INSTR';
    %DutAddr = 'TCPIP0::156.140.92.16';  %Example string for LAN
    
    %connects with instrument and configures
    myDmm = visa('agilent',DutAddr);
    set(myDmm,'EOSMode','read&write');
    set(myDmm,'EOSCharCode','LF') ;
    fopen(myDmm);
    
    % Example to query the instrument.
    fprintf(myDmm, '*IDN?');
    idn = fscanf(myDmm)
    
    
    
    
    fprintf(myDmm,'CONF:VOLT:DC 1, 1e-4'); %Configure a semi-fast DCV measurement
    fprintf(myDmm,'SAMP:COUN 20'); %Get 20 Readings per trigger
    
    %Clear all calculations before we start
    fprintf(myDmm,'CALC:AVER:CLE');
    fprintf(myDmm,'CALC:AVER:STAT ON'); %Turn on Stat calculations for future readings
    
    %Set the limits and turn on internal limit checking
    fprintf(myDmm,'CALC:LIM:LOW -1e-3'); %Set lower limit to -1mV
    fprintf(myDmm,'CALC:LIM:UPP 1e-3'); %Set upper limit to -1mV
    
    fprintf(myDmm,'CALC:LIM:STAT ON'); %Turn on Limits for future readings
    
    %Get the readings
    fprintf(myDmm,'READ?'); %Trigger the DMM to start the readings, can also use INIT command
    DCVResult = fscanf(myDmm)
    
    %Read out the Questionable status register to determine programmatically if there was a limit failure
    fprintf(myDmm,'STAT:QUES?');
    status = fscanf(myDmm,'%i');
    
    %if Bit 12 from status register is set then the upper limit failed
    uppLimFail = bitand(4096, status);
    %If bit 11 from status register is set then the lower limit failed.
    lowLimFail = bitand(2048, status);
    
    if (uppLimFail == 4096)
        fprintf('A reading failed Upper limit!\n');
    end
    if (lowLimFail == 2048)
        fprintf('A reading failed Lower limit!\n');
    end
    
    %*Let's get some statistics from the measurements that we just took
    
    
    fprintf(myDmm,'CALC:AVER:MAX?'); %Get maximum from the readings
    max = fscanf(myDmm)
    CheckDMMError(myDmm); %Check DMM for error
    
    fprintf(myDmm,'CALC:AVER:MIN?'); %Get minimum from the readings
    min = fscanf(myDmm)
    CheckDMMError(myDmm); %Check DMM for error
    
    %Let's do some internal math on the readings
    fprintf(myDmm,'CALC:AVER:AVER?'); %Get average from the readings
    mean = fscanf(myDmm)
    CheckDMMError(myDmm); %Check DMM for error
    
    fprintf(myDmm,'CALC:AVER:SDEV?'); %Get standard dev from the readings
    sdev = fscanf(myDmm)
    CheckDMMError(myDmm); %Check DMM for error
    
catch MExc
    MExc.message
    delete_all_objs()
    
end


