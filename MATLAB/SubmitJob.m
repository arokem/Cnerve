function Expt = SubmitJob(Expt)
% SubmitJob: This function takes the Expt structure create by SFExptConfig
% and submits a job on Expt.Machine. This is principally useful for
% Machines that require the use of a job scheduler.

% Unpack necessary parameters from Expt structure.
dataDir         = Expt.fnames.DataFiles.path;
ExptName        = Expt.name;
ExptFilePath    = Expt.fnames.DataFiles.ExptFile;
SQLlist         = 'Multi';

numFibers       = Expt.univParams.numFibers;
fibersplitter   = Expt.opts.fibersplitter;
monte           = Expt.opts.monte;

switch Expt.Machine
    case 'Julep'
        cd(dataDir);
%         Status = unix('parallel -j 4 --eta -a commands');
%         [Expt,SpikesData,SpikeNodeData,VoltageData] = retrieveFinished(Expt);
%         save(dataFilePath,'Expt','SpikesData','SpikeNodeData','VoltageData','-v7.3')
        
    case 'Hyak'
        %Calculate nodes to request
        singleStimRuntime = 100/3.2*1.5; %seconds, a rough estimate. Should be done dynamically in future.
        
        %Need to alter constant depending on simulation length.
        estJobTime = numFibers/fibersplitter*monte*singleStimRuntime; 
        
        % Divide 120 min by length of job in minutes to give jobs needed per core.
        jobsPercore = floor(180/estJobTime*60); 
        assert(jobsPercore > 0,'Too many fibers per job.');
        ppn = 8; % Processors/node.
        
        % How many nodes to request.
        nodesNeeded = ceil(Expt.idx1Max*Expt.idx2Max*Expt.idx3Max*fibersplitter/jobsPercore/ppn);
        mem = 1.5*ppn; % Memory requested Gb/core.
        
        %Calculate walltime
        buffer = 60; %Minutes
        walltime = ceil(jobsPercore * estJobTime/60 + buffer); %Minutes
        
        fileID = fopen(sprintf('%sparallel_sql_job.pbs',dataDir),'w');
        fprintf(fileID,'#!/bin/bash\n');
        fprintf(fileID,'#\n');
        fprintf(fileID,'#PBS -N %s\n',ExptName);
        fprintf(fileID,'#PBS -l nodes=1:ppn=%d,mem=%dgb,feature=%dcore\n',ppn,mem,ppn);
        fprintf(fileID,'#PBS -l walltime=00:%d:00\n',walltime);
        fprintf(fileID,'#PBS -o %sOutput\n',dataDir);
        fprintf(fileID,'#PBS -j oe\n');
        fprintf(fileID,'#PBS -d %s\n',dataDir);
        fprintf(fileID,'#\n');
        fprintf(fileID,'cd $PBS_O_INITDIR\n');
        fprintf(fileID,'HYAK_SLOTS=`wc -l < $PBS_NODEFILE`\n');
        fprintf(fileID,'module load parallel_sql\n');
        fprintf(fileID,'runcommand()\n');
        fprintf(fileID,'{\n');
        fprintf(fileID,'parallel-sql --sql --sql-set %s -a parallel --exit-on-term -j $HYAK_SLOTS\n',SQLlist);
        fprintf(fileID,'}\n');
        fprintf(fileID,'waituntilkill()\n');
        fprintf(fileID,'{\n');
        fprintf(fileID,'while true;\n');
        fprintf(fileID,'do\n');
        fprintf(fileID,'  sleep 1\n');
        fprintf(fileID,'done\n');
        fprintf(fileID,'}\n');
        fprintf(fileID,'trap ''waituntilkill'' SIGTERM SIGINT\n');
        fprintf(fileID,'runcommand\n');
        fclose(fileID);
        
        %% Create MATLAB retrieval PBS file
        fileID = fopen(sprintf('%sretrieve.pbs',dataDir),'w');
        fprintf(fileID,'#!/bin/bash\n');
        fprintf(fileID,'#\n');
        fprintf(fileID,'#PBS -N %s_retrieval\n',ExptName);
        fprintf(fileID,'#PBS -l nodes=1:ppn=%d,mem=%dgb,feature=%dcore\n',ppn,mem,ppn);
        fprintf(fileID,'#PBS -l walltime=03:00:00\n');
        fprintf(fileID,'#PBS -o %sRetrieveOut\n',dataDir);
        fprintf(fileID,'#PBS -j oe\n');
        fprintf(fileID,'#PBS -d /gscratch/stf/jresnick/CnerveRepo/Simulation/MATLAB/Retrieval\n');
        fprintf(fileID,'#\n');
        fprintf(fileID,'module load matlab_2015b\n');
        fprintf(fileID,'#\n');
        fprintf(fileID,'cd $PBS_O_INITDIR\n');
        fprintf(fileID,'matlab -nosplash -nodisplay <<retrieveSub\n');
        fprintf(fileID,'retrieve(''%s'');\n',ExptFilePath);
        fprintf(fileID,'exit;\n');
        fprintf(fileID,'retrieveSub\n');
        fclose(fileID);
        
        %% Below qsubber.sh submission script
        
        fileID = fopen(sprintf('%sqsubber.sh',dataDir),'w');
        fprintf(fileID,'#!/bin/bash\n');
        fprintf(fileID,'#\n');
        % Load commands into sql database.
        fprintf(fileID,'module load parallel_sql\n');
        fprintf(fileID,'cd %s\n',dataDir);
        fprintf(fileID,'psu --del --sql-set %s -y\n',SQLlist);
        fprintf(fileID,'cat commands | psu --load --sql-set %s\n',SQLlist);
        fprintf(fileID,'#\n');
        % Call the apropriate number of parallel-sql calls.
        fprintf(fileID,sprintf('for job in $(seq 1 %d); do SimJobs+="$(qsub -q bfwait ./parallel_sql_job.pbs):"; done\n',nodesNeeded));
        fprintf(fileID,'SimJobs=$(echo -n $SimJobs | head -c -1)\n');
        fprintf(fileID,'echo $SimJobs\n');
        % Call retrieval script after all simulations are done.
        fprintf(fileID,'qsub -q bf -W depend=afterok:$SimJobs ./retrieve.pbs\n');
        fprintf(fileID,'exit 0');
        fclose(fileID);
        
        cd(dataDir);
        fileattrib('*.sh','+x');
        fileattrib('*.pbs','+x');      
end
end