function [Expt,SpikesData,SpikeNodeData,VoltageData,areAllDone] = retrieveFinished(Expt, doEndianCheck)
%
% Sample usage:
%		[Expt,SpikesData,SpikeNodeData,VoltageData,areAllDone] = retrieveFinished(Expt, doEndianCheck)
%
% Description:
%	Gathers results from hyak from experiment 'Expt', calls programs to
%	read in all of the returned files, processes them back into the
%	SpikesData, SpikeNodeData, and VoltageData arrays, and returns.  If the
%	individual job inside the 'expt' has failed, under certain conditions,
%	it resubmits the job. Don't forget to clean the hyak queue after the
%	experiment has been processed and all the results gathered back in
%	'expt' structure.
%
%
%
% INPUTS:
%	doEndianCheck	: 0/1 for whether to try to read endian indicator at
%					  head of file
%
% OUTPUTS:
%	areAllDone		: (bool) indicating if all jobs in the experiments are
%					  done.

%
%
% Author:
%           Jesse Resnick
%			Nikita Imennov <imennov@uw>
%


areAllDone	= 0;		% let's just assume that things won't work out


if ~exist('doEndianCheck', 'var')
    doEndianCheck 		= 1;
end


fprintf('Retrieving status of jobs');
% Just retrieve the status of jobs we know are not done yet (in case we
% need to rerun retrieveFinishedJobs, this will save us some time...
%jobsWeWant.jids	= setdiff(expt.hyak.allJIDs, expt.hyak.doneJIDs);
%jobsWeWant.isDone	= jobIsDone( jobsWeWant.jids );

% leave only JIDs with valid (i.e. non-zero) values
%jobsWeWant.doneJIDs	= jobsWeWant.jids .* jobsWeWant.isDone;
% remove zero values from the list
%jobsWeWant.doneJIDs = setdiff(jobsWeWant.doneJIDs, 0);

%if isempty(jobsWeWant.doneJIDs)
%	return
%end
%fprintf('Done.\n');


%fprintf('Retrieving results of JID #:');
%failedJIDs = jobResults(jobsWeWant.doneJIDs, './');


%%% if fixing a broken pull from hyak, comment out the preceeding section, put
%%% broken (but pulled) jobs into brokenJIDs, and the following lines after the
%%% commented out block:
%%
%%jobsWeWant.jids		= expt.brokenJids;
%%jobsWeWant.doneJIDs	= expt.brokenJids;

fprintf('\tProcessing retrieved results:');

% for whatever reason, exist() doesn't work to check, so using isfield
% instead. here I'm assuming all trials have the same options as
% 1,1,1...which tends to be true.

% Pull out critical option parameterrs from Expt structure.
doRecordVoltage = ( (isfield(Expt, 'opts') && Expt.opts.RecordVoltage ) );
% doRecordECAP	= ( (isfield(expt.trial{1,1,1}, 'opts') && expt.trial{1,1,1}.opts.RecordECAP ) );


MaxSpikes   = Expt.opts.MaxSpikes;
idx1Max     = Expt.idx1Max;
idx2Max     = Expt.idx2Max;
idx3Max     = Expt.idx3Max;
Fibers      = Expt.univParams.numFibers;
Montes      = Expt.opts.monte;
MaxNodes    = Expt.fibers.maxNodes;
TimeSteps   = sum(Expt.stim.trial{1,1,1}.pulselist(:,2));
Delt        = Expt.opts.delt;
MaxSpikes   = Expt.opts.MaxSpikes;

% Initialize data structures.
SpikesData      = NaN(idx1Max,idx2Max,idx3Max,Fibers,Montes,MaxNodes,MaxSpikes);
SpikeNodeData   = NaN(idx1Max,idx2Max,idx3Max,Fibers,Montes,MaxSpikes);

if(doRecordVoltage)
    VoltageSample = Expt.opts.VoltageSample;
    VoltageData   = NaN(idx1Max,idx2Max,idx3Max,Fibers,Montes,MaxNodes,round(TimeSteps*Delt/VoltageSample));
end

for idx1=1:idx1Max
    for idx2=1:idx2Max
        for idx3=1:idx3Max
            for idxFiberSplitter=1:size(Expt.jobs,4)
                aJob = Expt.jobs{idx1,idx2,idx3,idxFiberSplitter};
                % spikes file (.spks), output
                spksFilename = [aJob.outputfilename Expt.fnames.spksSuffix];
                
                try
                    [tmpfibers, tmpfiblist, numNodesSpks, tempmcr, ...
                        Expt.stim.trial{idx1,idx2,idx3}.nstep, ...
                        Expt.stim.trial{idx1,idx2,idx3}.delt, tempspikes] = readSpikeTimes(spksFilename, doEndianCheck);
                catch e
                    fprintf('\tWARNING: readSpikeTimes(%s): %s\n', spksFilename, e.message);
                    %warning('Continuing with loop regardless!');
                    continue;
                end
                
                if(tmpfibers <= 0)
                    % job we've just retrieved obviously failed, it
                    % returned <= 0 fibers, so lets resubmit it and move
                    resubmitJob();
                    continue;
                end
                
                % ok, so far so good; feedback to the user that we're getting
                % closer to the (eventual) end of data processing
                fprintf('.');
                
                
                if(doRecordVoltage)
                    % voltage file (.vltg)
                    vltgFilename = [aJob.outputfilename Expt.fnames.vltgSuffix];
                    
                    try
                        if exist('expectXFibers', 'var')
                            [tmpfibersVolt, tmpfiblistVolt, numNodesVolt, ...
                                Expt.stim.trial{idx1,idx2,idx3}.nstep,...
                                ~,...
                                Expt.stim.trial{idx1,idx2,idx3}.delt,...
                                Expt.stim.trial{idx1,idx2,idx3}.vltgTime,tempvdata] = ...
                                readVoltFile(vltgFilename, doEndianCheck, VoltageSample,expectXFibers);
                        else
                            [tmpfibersVolt, tmpfiblistVolt, numNodesVolt, ...
                                Expt.stim.trial{idx1,idx2,idx3}.nstep,...
                                ~,...
                                Expt.stim.trial{idx1,idx2,idx3}.delt,...
                                Expt.stim.trial{idx1,idx2,idx3}.vltgTime,tempvdata] = ...
                                readVoltFile(vltgFilename, doEndianCheck,tempmcr,VoltageSample);
                        end
                        
                        if tmpfibersVolt ~= tmpfibers
                            fprintf('\tWARNING: Volt and Spike files give conflicting fiber count (%f vs %f)!\n', ...
                                tmpfibersVolt, tmpfibers);
                            clear tmpfibersVolt;
                        end
                        if tmpfiblistVolt ~= tmpfiblist
                            fprintf('\tWARNING: Volt and Spike files give conflicting fiblist count (%f vs %f)!\n', ...
                                tmpfiblistVolt, tmpfiblist);
                            clear tmpfiblistVolt;
                        end
                        if numNodesVolt ~= numNodesSpks
                            fprintf('\tWARNING: Volt and Spike files give conflicting nodnums count (%f vs %f)!\n', ...
                                numNodesVolt, numNodesSpks);
                            clear tmpnodnumsVolt;
                        end
                        
                    catch e
                        fprintf('\n\tWARNING: readVoltFile(%s): %s\n', vltgFilename, e.message);
                        continue;
                    end
                end % if(doRecordVoltage)
                
                % %                 if(doRecordECAP)
                % %                     % ecap file (.ecap)
                % %                     ecapFilename = [aJob.outputfilename expt.fnames.ecapSuffix];
                % %
                % %                     % so, am i overriding 'tmpfiblist' read from
                % %                     % readSpikeTimes() with this one?!?
                % %                     %
                % %                     % why is tempecap not being saved?
                % %
                % %                     try
                % %                         [expt.trial{idx1,idx2,idx3}.nstep, tempvltgTime, tmpfiblist, tempecap] = readECAPFile(ecapFilename, expt.decimator, doEndianCheck);
                % %                     catch e
                % %                         % XXX: expand to be more verbose
                % %                         fprintf('\nError executing readECAPFile(%s).	Continuing next iteration.', ecapFilename);
                % %                         continue;
                % %                     end
                % %
                % %
                % %                     % adjust time scale?
                % %                     expt.trial{idx1,idx2,idx3}.vltgTime = tempvltgTime*expt.trial{idx1,idx2,idx3}.delt;
                % %
                % %                     clear tempvltgTime
                % %                 end % if(doRecordECAP)
                
                for n = 1:length(tmpfiblist)
                    if(doRecordVoltage)
                        VoltageData(idx1,idx2,idx3,tmpfiblist(n),:,1:numNodesSpks(n),:) = tempvdata(n,:,1:numNodesSpks(n),:);
                        
                        clear tempvdata{n}				% usually takes a lot of space
                    end
                    
                    %                                         if(doRecordECAP)
                    %                                             try
                    %                                                 expt.trial{idx1,idx2,idx3}.ecap = expt.trial{idx1,idx2,idx3}.ecap + tempecap;
                    %                                             catch
                    %                                                 expt.trial{idx1,idx2,idx3}.ecap = zeros(size(tempecap));
                    %                                                 expt.trial{idx1,idx2,idx3}.ecap = expt.trial{idx1,idx2,idx3}.ecap + tempecap;
                    %                                             end
                    %                                         end
                    
                    SpikesData(idx1,idx2,idx3,tmpfiblist(n),:,1:numNodesSpks(n),:)= tempspikes(n,:,1:numNodesSpks(n),:);
                    RecordNode = round(Expt.opts.nodeint.*numNodesSpks(n));
                    SpikeNodeData(idx1,idx2,idx3,tmpfiblist(n),:,:) = SpikesData(idx1,idx2,idx3,tmpfiblist(n),:,RecordNode,:);
                    
                end % for n = 1:length(tmpfiblist)
                
                
                % ok, if we got here, we're assuming all is well.  transfer JID to done JID
                
                %expt.hyak.doneJIDs = union(expt.hyak.doneJIDs, aJob.jid);
                
                
                % explicitly clear the memory:
                clear temp* tmp*
                
                
                % WARNING: ASSUMING ALL IS WELL, BUT NOT REALLY SURE HOW VALID
                % THAT ASSUMPTION IS!!!
                %warning('DELETETING PRODUCED FILES!!!');
                %fprintf('deleting %s, %s, %s\n', spksFilename, vltgFilename, ecapFilename);
                %delete(spksFilename);
                %delete(vltgFilename);
                %delete(ecapFilename);
                
                % ok, instead of deleting the files, at least compress them:
                % unix(['bzip2 --compress --force ' aJob.outputfilename '*']);
                
                % append histogram from this fiber-split result file into a global fiber histogram
                %%% expt.trial{idx1,idx2,idx3}.hist_node = [expt.trial{idx1,idx2,idx3}.hist_node tmpHistNode];
                
                
            end % for idxFiberSplitter=1:size(expt.hyak.jobs,4)
            
            
        end %end idx3 loop
    end %end idx2 loop
end %end idx1 loop

fprintf('Done.\n');


%areAllDone = isempty(  setxor(expt.hyak.doneJIDs, expt.hyak.allJIDs)  );


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    function resubmitJob()
        %
        % Description:
        %	Resubmits a failed job w/a different seed, but the rest of the arguments
        %	the same.  Updates experiment's (expt) .hyak.allJIDs and .hyak.doneJIDs
        %
        
        
        % presumably, we failed last time due to some minima we got stuck in. a
        % way to fix that is to change the seed on the restarted job:
        %% TOO LIMITED: expt.hyak.jobs{idx1,idx2,idx3,idxFiberSplitter}.seed = aJob.seed+10;
        
        % generate negative ints throughout the whole valid neg int32 interval:
        Expt.hyak.jobs{idx1,idx2,idx3,idxFiberSplitter}.seed = floor(  double(  ...
            intmin('int32')+1 ...
            ) .* rand() ...
            ) - 1;
        
        
        command = sprintf(  'cnerve %s %s %s %s %d %d %d %d %d ', ...
            aJob.modlFilename, aJob.stimFilename, aJob.optsFilename, ...
            aJob.outputfilename, ...
            aJob.seed, aJob.mcrper, aJob.numfibs, ...
            aJob.firstfib, aJob.fibskip  );
        
        % if the original job had an ART script, use one during resubmit as well:
        if isfield(aJob, 'artScript')
            resubmitJID = jobSubmit(command, aJob.artScript);
        else
            resubmitJID = jobSubmit(command);
        end
        
        oldJID		= aJob.jid;
        
        %fprintf('\tWARNING: Resubmitted job %d because it failed.  New id: %d.\n', oldJID, resubmitJID);
        fprintf('[resubmit JID %d-->%d]', oldJID, resubmitJID);
        
        jobDelete(oldJID);			% remove from hyak's list of jobs
        
        % replace failed job from .hyak.allJIDs with resubmited
        % job; add failed (but finished) job to .hyak.doneJIDs
        Expt.hyak.allJIDs	= setxor(Expt.hyak.allJIDs, [oldJID resubmitJID]);
        
        
        Expt.hyak.jobs{idx1,idx2,idx3,idxFiberSplitter}.jid = resubmitJID;
        
        
    end % function resubmitJob()


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    function areDone = jobIsDone(jidList)
        
        % returns a list of booleans corresponding to whether the hyak job (numbers
        % passed in jidList argument) is still running
        
        areDone = zeros(size(jidList));		% by default, assume all jobs are running
        
        failedJIDs.other	= [];			% list of jids that failed to stat for some reason
        failedJIDs.invalid	= [];			% list of jids that are invalid, according to hyak
        failedJIDs.parse	= [];			% list of jids whose responses we didn't understand
        
        %%%% fprintf('...');
        
        for i = 1:length(jidList)
            
            [status, attributes] = unix(  sprintf('hyak -job attributes -id %d', jidList(i))  );
            
            
            if(status ~= 0)
                %%try
                %%% do not use assert, causes problems -- matlab crashes
                %%	assert(status == 0);
                %%catch e
                % error opening file, likely.  just go on to the next one.
                %warning('Cannot open JID''s (%d) attributes; skipping\n', jidList(i));
                areDone(i) = 0;
                failedJIDs.other = [failedJIDs.other jidList(i)];
                continue;
            end
            
            undoneTasks = regexp(attributes, '(?<=undoneTaskCount\s\=\s)\d+', 'match');
            
            if isempty(undoneTasks)
                % regexp problem.  assume still running
                areDone(i) = 0;
                
                % but, see if it's an invalid job (most common problem)
                if ~isempty(regexp(attributes, 'invalidJobIdentifier', 'once'))
                    failedJIDs.invalid = [failedJIDs.invalid jidList(i)];
                end
                
                %warning('Cannot parse JID''s (%d) attributes; skipping\n', jidList(i));
                continue;
            end
            
            areDone(i) = ~str2double(undoneTasks{1});
            
            %%%%% presumably, we will get called by retrieveFinishedJobs, so just print
            %%%%% out dots as we are reading in job's statuses...
            
            fprintf('.');
            %%%%	% although neater, erasing the dots makes it hard to monitor progress
            %%%%	% when the jobs are large
            %%%% 		switch mod(i, 3)
            %%%% 			case 1,
            %%%% 				fprintf('\b\b\b.  ');
            %%%% 			case 2,
            %%%% 				fprintf('\b\b\b.. ');
            %%%% 			case 0,
            %%%% 				fprintf('\b\b\b...');
            %%%% 			otherwise,
            %%%% 				fprintf('\b\b\b???');
            %%%% 		end
            %%%%
            
        end
        
        %% PRINT OUT STATUS
        %%%%	fprintf('\b\b\b:\n');
        fprintf('\n');
        if failedJIDs.other				% notify user of failures
            fprintf('\tWARNING: could not open attributes for JIDs: ');
            fprintf(' %d', failedJIDs.other);
            fprintf('\n');
        end
        if failedJIDs.invalid			% tell which jobs were invalid
            fprintf('\tWARNING: hyak didn''t know these JIDs:');
            fprintf(' %d', failedJIDs.invalid);
            fprintf('\n');
        end
        if failedJIDs.parse				% which were weird about their status?
            fprintf('\tWARNING: couldn''t parse status of:');
            fprintf(' %d', failedJIDs.parse);
            fprintf('\n');
        end
        
        if sum(areDone)
            fprintf('\t%d of %d jobs we need are done, retrieving their results...\n', ...
                sum(areDone), length(jidList));
        else
            fprintf('\t%d of %d jobs we need are done.\n', ...
                sum(areDone), length(jidList));
        end
        
    end % function areDone = jobIsDone(jidList)



end % function [expt, areAllDone] = retrieveFinishedJobs(expt)
