function [fibers, fiblist, nodnums, mcr, nstep, delt, spikes] = readSpikeTimes(filename, doEndianCheck)
% function [fibers, fiblist, nodnums, mcr, nstep, delt, spikes] = readSpikeTimes(filename, doEndianCheck, expectXFibers)
%
% Sample usage:
%
%	[fibers, fiblist, nodnums, mcr, nstep, delt, spikes] = ...
%				readSpikeTimes('default.spks', doEndianCheck, expectXFibers);
%
%	[a.fibers, a.fiblist, a.nodnums, a.mcr, a.nstep, a.delt, a.spikes] = readSpikeTimes(filename, doEndianCheck)
%	
% Description:
%	Reads the file written by C program (___)
%
%
%
% INPUTS:
%	doEndianCheck	: 0/1 for whether to try to read endian indicator at
%					  head of file
%
% TODO: description
%
% Returns:
%	fibers			: (int), # of fibers read.  -1 if C program returned an error
%	fiblist			:
%	nodnums			: 
%	mcr				: (int), # of Monte Carlo simulations performed
%


fiblist = [];
nodnums = -1;
mcr		= -1;
nstep	= -1;
delt	= -1;
spikes	= {};

try
	if exist('doEndianCheck', 'var') && doEndianCheck

		endianFmt	= checkEndianness(filename);	% either 'ieee-le' or 'ieee-be' or error

		[fid]	= fopen(filename, 'r', endianFmt);
		fread(fid, 1, 'uint32');						% skip the endianness indicator

	else
		[fid]	= fopen(filename, 'r', 'ieee-be');
	end
catch e
	fclose(fid);
	rethrow(e);
	% assert(fid > 0, 'Cannot open file %s for reading (error: %s)', filename, message);
end

try
	mcr		= fread(fid, 1, 'int32');
	fibers	= fread(fid, 1, 'int32');
catch e
	fclose(fid);
	rethrow(e);
end

if (fibers > 1000) || (fibers <= 0)
	% we get -1 fibers if the voltage exceeds 1500, as per C code.  (This
	% statement is current as of this checkin.  No guarantees)
	fibers = -1;
	fclose(fid);
	return;
end

% from now on, we're assuming the jobs didn't fail, so we'll continue reading
% w/o error checking.  that's pretty bad

maxnodes= fread(fid, 1,		'int32=>int32');

fiblist	= fread(fid, fibers, 'int32=>int32')';


% maximum number of nodes for each fiber
nodnums	= fread(fid, fibers, 'int32=>int32')';


MaxSpikes	= fread(fid,1,'int32=>int32');
nstep		= fread(fid,1,'int32=>int32');
delt		= fread(fid,1,'double=>single');

spikes		= zeros(fibers,mcr,maxnodes,MaxSpikes);

for k=1:mcr
	for i=1:fibers
		for j=1:maxnodes 
			spikes(i,k,j,:) = single(delt*fread(fid, double(MaxSpikes),'uint=>single'));     
		end
	end
end

fclose(fid);
end


