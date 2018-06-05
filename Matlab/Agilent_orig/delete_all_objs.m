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


function delete_all_objs
%DELETE ALL OBJS
%Usage
%  Clean up all instrument objects not properly closed.
%  Run once at beginning of script or at the end to clean up objects
%
newobjs = instrfind;
if isempty(newobjs) == 0 
  % if using device objects may need to disconnect instead of close
  % disconnect(newobjs)
  fclose(newobjs);
  delete(newobjs);
  clear newobjs
end