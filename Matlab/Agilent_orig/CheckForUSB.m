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


%This function is used with the FileManage.m example program file for the
%34460A and 34461A

%This is an example of a function to do a quick check if a USB thumb is
%attached to the DMM
function CheckUSB = CheckForUSB(myDmm)
%First check if the USB Stick is on the DMM

fprintf(myDmm,'MMEM:CDIR "USB:\\"'); %Change default directory to USB if we can

fprintf(myDmm,'SYST:ERR?');
errStr = fscanf(myDmm);
if (strfind(errStr,'+0')) %if no error then USB is available and continue
    CheckUSB = true;
else if (strfind(errStr,'-256')) %-256 is the error code for file or folder not found
        
        %Clear out any additional errors
        errStr2 = errStr;
        NoErr = strfind(errStr2,'No error');
        while (isempty(NoErr));
            fprintf(myDmm,'SYST:ERR?');
            errStr2 = fscanf(myDmm);
            NoErr = strfind(errStr2,'No error');
            if (isempty(NoErr))
                errStr = strcat(errStr, '\n', errStr2);
            end
        end
        %Ask the user if they want to continue with USB test
        fprintf('No USB thumb drive connected. Please install one. If you choose not to install one, this program will exit\n');
        fprintf('Press any key to continue\n');
        
        pause
    end
end

fprintf(myDmm,'MMEM:CDIR "USB:\\"');
%Check for an error again, if there is an error -256 then we return a false for USB
fprintf(myDmm,'SYST:ERR?');
errStr = fscanf(myDmm);
if (strfind(errStr,'+0')) %if no error then USB is available and continue
    CheckUSB = true;
else if (strfind(errStr,'-256')) %-256 is the error code for file or folder not found
        
        %Clear out any additional errors
        errStr2 = errStr;
        NoErr = strfind(errStr2,'No error');
        while (isempty(NoErr));
            fprintf(myDmm,'SYST:ERR?');
            errStr2 = fscanf(myDmm);
            NoErr = strfind(errStr2,'No error');
            if (isempty(NoErr))
                errStr = strcat(errStr, '\n', errStr2);
            end
        end
        CheckUSB =  false;
        
    else
        
        CheckDMMError(myDmm);
        CheckUSB = false;
    end
    
end
fprintf(myDmm,'SYST:ERR?');
errStr = fscanf(myDmm);

if (strfind(errStr,'No error')) %If no error, then return
    return;
    %If there is an error, read out all of the errors and return them in an exception
else
    errStr2 = errStr;
    NoErr = strfind(errStr2,'No error');
    while (isempty(NoErr));
        fprintf(myDmm,'SYST:ERR?');
        errStr2 = fscanf(myDmm);
        NoErr = strfind(errStr2,'No error');
        if (isempty(NoErr))
            errStr = strcat(errStr, '\n', errStr2);
        end
    end
    errStr = strcat('Exception: Encountered system error(s)\n',errStr)
    err = MException('ResultChk:BadInput',errStr);
    throw(err)
end

end