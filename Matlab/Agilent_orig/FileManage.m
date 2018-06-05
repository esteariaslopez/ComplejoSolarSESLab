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


%  /*Four use cases using the MMEM subsystem. This subsystem contains the SCPI file management commands that can be used to create/copy/paste files.
%          * 1) Takes 10 readings. Then stores the data from reading memory and copies it to a file on the internal DMM drive. It then shows how to copy
%          * that data over to the host PC.
%          * 2) Takes the current state of the DMM then saves the preference and state into files on the internal DMM drive. It then resets the DMM and recalls
%          * the state and preference files to bring back the previous state of the DMM
%          * 3) we transfer files from the PC to the DMM internal drive and the USB stick (if there is a USB stick installed)
%          * 4) Erase all the files that were created.
%          */
%
%         /*'This program does not exhibit the most robust error handling. This means that there could be cases where the program will not run due to file errors. This
%         'is done on purpose to keep the program simple so that you can read and understand it.
%          * */

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
    %fprintf(idn)
    
    % /*Example 1: Take 10 resistance readings and report them to the screen.
    %                  * Then save the readings from the buffer into an internal file on the DMM.
    %                  * Finally transfer the file from the DMM to the PC and store in C:\Temp
    %                  * */
    fprintf('Example 1: Get readings from DMM, also store readings from DMM into a file on DMM');
    %Configure for Resistance 1000ohm range, 10mV resolution this is a fast reading
    fprintf(myDmm,'CONF:RES 1000, 0.01');
    fprintf(myDmm,'SAMP:COUN 10'); %Get 10 Readings per trigger
    fprintf('\nReadings direct from DMM: ');
    fprintf(myDmm,'READ?');%Trigger the DMM to start the readings, can also use INIT and FETCH commands
    OhmResult = fscanf(myDmm); %Get the readings in an array
    %Code to get the readings into a double array format
    %                 while any(remainder)
    %                     [chopped,remainder] = strtok(remainder);
    %                     result = [result, str2num(chopped)];
    %                 end
    
    %Check for any DMM errors
    CheckDMMError(myDmm);
    
    %Transfer/Save the readings into Internal disk
    fprintf(myDmm,'MMEM:STOR:DATA RDG_STORE, "INT:\MyOhmsRdgs"'); %Creates and stores readings into MyOhmssRdgs.csv
    
    %Let's read out the readings from the file into the console
    fprintf(myDmm,'MMEM:UPL? "INT:\MyOhmsRdgs.csv"');
    
    %The readings are now in a file, so we need to read out as bytes, then convert to a string for printout
    FileRdgs = binblockread(myDmm,'uint8');
    fread(myDmm,1);%remove the EOS character from buffer, without this command the DMM will throw an error
    StrFileRdgs = native2unicode(FileRdgs,'UTF-8')'; %Convert the characters from UTF-8 format, transpose the measurement array so it is readable
    
    
    fprintf('\nReadings from File:\n');
    fprintf(StrFileRdgs);
    
    %Check for any DMM errors
    CheckDMMError(myDmm);
    
    %Now that we have the file read back, let's save the file on our PC
    OutFilename = 'C:\Temp\OhmsReadings.txt'; %Set file location
    f = fopen(OutFilename, 'w'); %Create/Overwrite the local copy of the file
    fwrite(f,FileRdgs,'uint8'); %Write the file data
    %fClose(f); %Close the file
    
    fprintf('Ohms readings saved at: %s\n',OutFilename);
    fprintf('Press any key to continue to Example 2\n');
    pause
    %                 /*Example 2
    %                  * Take the current state of the DMM then saves the preference and state into files on the internal DMM drive.
    %                  * Then resets the DMM and recalls the state and preference files to bring back the previous state of the DMM
    %                  *      State files contain measurement specific instrument states such as Function, range, trigger settings etc.
    %                  *      Preference files contain DMM system settings such as IO addresses, fprintflay brightness, power-on state etc.
    %                  */
    fprintf('Example 2: Create State files from DMM, reset DMM, then recall the state from the State files\n');
    fprintf(myDmm,'CONF?');
    currConf = fscanf(myDmm);
    fprintf('Current Configuration is: %s\n', currConf);
    fprintf(myDmm,'MMEM:STOR:STAT "INT:\OhmsStat"'); %Store the DMM state into an internal file, it automatically stores as .sta file
    fprintf(myDmm,'MMEM:STOR:PREF "INT:\OhmsPref"'); %Store the DMM preferences into an internal file, automatically stores as .prf file.
    
    %reset the DMM and prove that the state has changed
    fprintf(myDmm,'*RST');
    fprintf(myDmm,'CONF?');
    RstConf = fscanf(myDmm);
    fprintf('Reset Configuration is: %s\n', RstConf);
    
    %Load the state and preference files from out previous state
    fprintf('Recalling State and preferences into DMM\n');
    fprintf(myDmm,'MMEM:LOAD:STAT "INT:\OhmsStat.sta"'); %Recall the DMM state from the internal file
    %fprintf(myDmm,'MMEM:LOAD:PREF "INT:\OhmsPref.prf"'); %Recall the DMM preferences from the internal file, this is commented out because it will reboot the DMM.
    
    fprintf(myDmm,'CONF?');
    LoadConf = fscanf(myDmm);
    fprintf('Reloaded Configuration is: %s\n', LoadConf);
    fprintf('Press any key to continue to Example 3\n');
    pause
    
    %                 /*Example 3: Transfer files from PC to DMM internal drive and USB stick.
    %                  * Get the OhmsReading.txt file store in C:\Temp from the previous example and copy it to a new directory on the DMM drive
    %                  * Check if the DMM has a USB drive, if not then do a single prompt to install it. If the second check doesn't find a USB stick skip the USB stick portion
    %                  * If the USB stick is installed then create a new folder and copy the File to the USB drive.
    %                  * Finally, delete all of the files that were created from this example.*/
    
    fprintf('Example 3: Copy files from PC to DMM\n');
    
    %Create a new folder on the DMM
    fprintf(myDmm,'MMEM:MDIR "INT:\Rdgs"');
    %
    %Specifying a filename on the DMM to download our new file to
    fprintf(myDmm,'MMEM:DOWN:FNAM "INT:\Rdgs\MyOhmsRdgs.csv"');
    
    %Check DMM Errors
    CheckDMMError(myDmm);
    
    %Read out the file from the PC
    %Now that we have the file read back, let's save the file on our PC
    f = fopen(OutFilename, 'r'); %Create/Overwrite the local copy of the file
    OhmByte = fread(f,'uint8'); %Read the file data
    %fClose(f); %Close the file
    
    binblockwrite(myDmm,OhmByte,'MMEM:DOWN:DATA '); %Download the data in IEEE format
    %Check DMM Errors
    CheckDMMError(myDmm);
    
    %Check if the file was downloaded correctly
    fprintf(myDmm,'MMEM:CAT? "INT:\Rdgs"');
    CatDMM = fscanf(myDmm);
    fprintf('INT:\\Rdgs\\ contains: %s\n',CatDMM);
    
    
    %Now let's do the same operations for the USB Drive
    %                 %First Check if there is a USB drive installed on the DMM
    DoUSBtest = CheckForUSB(myDmm);   %Uses the function CheckForUSB (included in the example programs package) to check if a USB stick is on the DMM
    %If available, we do the %USB test,
    %if not then we skip
    if (DoUSBtest == 1)
        %Create a new folder on the DMM
        fprintf(myDmm,'MMEM:MDIR "USB:\Rdgs"');
        CheckDMMError(myDmm); %Check for errors
        
        %Specifying a filename on the DMM to download our new file to
        fprintf(myDmm,'MMEM:DOWN:FNAM "USB:\Rdgs\MyOhmsRdgs.csv"');
        CheckDMMError(myDmm); %Check for errors
        
        %Finally write to the file
        binblockwrite(myDmm,OhmByte,'MMEM:DOWN:DATA '); %Download the data in IEEE format
        CheckDMMError(myDmm); %Check for errors
        
        %Check if the file was downloaded correctly
        fprintf(myDmm,'MMEM:CAT? "USB:\Rdgs"');
        CatUSB = fscanf(myDmm);
        fprintf('USB:\\Rdgs\\ contains: %s\n ', CatUSB);
        
        
    end
    
    fprintf('Press any key to delete all files created by this example\n');
    pause
    
    %Example 4
    %Finally Delete all the files and folders that were created
    
    %Deletes the Ohms reading file from the base directory
    fprintf(myDmm,'MMEM:DEL "INT:\MyOhmsRdgs.csv"');
    
    %Delete the state and preference files that we created
    fprintf(myDmm,'MMEM:DEL "INT:\OhmsStat.sta"'); %Delete the DMM state file
    fprintf(myDmm,'MMEM:DEL "INT:\OhmsPref.prf"'); %Delete the DMM preferences file
    CheckDMMError(myDmm); %Check for error
    
    %Delete the INT .csv file that we created
    fprintf(myDmm,'MMEM:DEL "INT:\Rdgs\MyOhmsRdgs.csv"');  %Deletes the file from the DMM Rdgs directory
    
    CheckDMMError(myDmm); %Check for error
    
    %Then delete the directory, directory deletion is only allowed if the directory is empty
    fprintf(myDmm,'MMEM:RDIR "INT:\Rdgs"');
    CheckDMMError(myDmm); %Check for error
    
    if (DoUSBtest == true) %Only delete the files if we did the USB test
        %Delete the USB .csv file that we created
        fprintf(myDmm,'MMEM:DEL "USB:\Rdgs\MyOhmsRdgs.csv"');
        CheckDMMError(myDmm); %Check for error
        %Then delete the directory, directory deletion is only allowed if the directory is empty
        fprintf(myDmm,'MMEM:RDIR "USB:\Rdgs"');
        CheckDMMError(myDmm); %Check for error
        
    end
    
    
    
    
catch MExc
    MExc.message
    delete_all_objs()
    
end


