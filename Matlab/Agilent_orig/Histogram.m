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



%Programmatically sets the display mode to histogram views.
% Takes a sample of 500 readings per trigger and reports on the results while the display shows the distribution in histogram mode.
%Then downloads a screen capture to the PC. Screen capture illustrates IEEE block transfer method.

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
    
    %Set the Display to Histogram mode, DCV and get 100 readings
    
    %Configure for DCV 100V range, 10mV resolution this is a fast reading
    fprintf(myDmm,'CONF:VOLT:DC 100, 0.01');
    
    fprintf(myDmm,'DISP:VIEW HIST'); %Set to HISTogram view, options are NUMeric|HISTogram|TCHart|METer
    
    fprintf(myDmm,'SAMP:COUN 500'); %Get 500 Readings per trigger
    fprintf(myDmm,'READ?'); %Trigger the DMM to start the readings, can also use INIT command
    DCVResult = fscanf(myDmm)
    CheckDMMError(myDmm); %Check if the DMM has any errors
    
    %Here's where you can get a screen capture of the display
    fprintf(myDmm,'HCOP:SDUM:DATA:FORM PNG'); %Set the format to PNG or BMP
    fprintf(myDmm,'HCOP:SDUM:DATA?'); %Send the command to get the screen data
    junk = binblockread(myDmm); %Read the data back from DMM. DMM returns in IEEE block format
    fread(myDmm,1);%remove the EOS character from buffer, without this command the DMM will throw an error
    CheckDMMError(myDmm); %Check DMM for Errors
    
    OutFilename = 'ScreenCapture.png'; %Set picture location
    fid = fopen(OutFilename, 'w'); %Create/Overwrite the local copy of the file
    
    fwrite(fid,junk,'uint8'); %Write the picture data
    %fClose(fid); %Close picture file
    str = sprintf('DMM Display saved at: %s', OutFilename);
    disp(str)
    
    
catch MExc
    MExc
    delete_all_objs()
    
end


