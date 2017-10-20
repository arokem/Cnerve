function Expt = PathDefining(Expt)
% This function accepts inputs form experimental config files, creates the
% relevant directories for saving data, and passes that directory
% information back to main. It uses the Machine switch to select the
% platform specific directory information.

%	adding the path of this and other necessary directories, in case they
%	are not already in the user's path

switch Expt.Machine
    %   Define NewData directory for each machine type.
    case 'Julep'
        datadir  = '/media/datadrive/NewData/';
    case 'Hyak'
        datadir  = '/gscratch/stf/jresnick/NewData/';
    case 'Dell'
        datadir  = 'C:\Users\jsere\Documents\NewData\';
end

%--------------------------------------------------------------------------------
%  SAVE OUTPUT Extension INFORMATION TO EXPT STRUCTURE
%--------------------------------------------------------------------------------
Expt.fnames.vltgSuffix      = '.vltg';
Expt.fnames.spksSuffix      = '.spks';
Expt.fnames.ecapSuffix      = '.ecap';

% Create paths to data structure and directory for storing binary files.
prefix	= [datadir Expt.name filesep];
Expt.fnames.DataFiles.path = prefix;
Expt.fnames.DataFiles.ExptFile = [datadir Expt.name '.mat'];

reply = [];
while isempty(reply)
    if(~isdir(prefix))
        % directory where we want to save files for this experiment run
        % does not exist.  create it, copy the executable to run w/hyak,
        mkdir(prefix)
        % Navigate to C code repository.
        cd(Expt.fnames.ConfigFiles.path);
        cd(['..' filesep '..']);
        cdir = ['.' filesep 'C_code' filesep];
        % copy hyak executable & its source code auto-archived @ build time
        switch Expt.Machine
            case 'Julep'
                copyfile([cdir 'Cnerve/Default/Cnerve'],prefix);
            case 'Hyak'
                copyfile([cdir 'Cnerve/Code_v2.6.2DemyelinPeri/Cnerve'],prefix);
            case 'Dell'
                copyfile([cdir 'Cnerve\Default\Cnerve.exe'],prefix);
        end     
        reply = 'OK, EXIT THE LOOP NOW';
    else
        reply = input( sprintf('Directory %s already exists, would you like to overwrite it? y/n [n] ', prefix), ...
            's' );
        if isempty(reply)
            reply = 'n';
        end
        
        if reply == 'y'
            rmdir(prefix,'s');
            reply = '';			% JUST ONE MORE ROUND ABOUT THE LOOP';
            continue;
        else
            error('Directory %s already exists, NOT re-running cnerve', prefix);
        end
        
    end % if(~isdir(prefix))
end % while isempty(reply)
end % Function PathDefining
