function [fibers, fiblist, nodnums, nstep, E_rest, delt, time, voltages] = readVoltFile(filename, doEndianCheck,Montes,VoltageSample,expectXFibers)
%
% Sample usage:
%	[tmpfibers, tmpfiblist, tmpnodnums, expt.trial{i,j,k}.nstep,...
%		expt.trial{i,j,k}.E_rest,  expt.trial{i,j,k}.delt,...
%		expt.trial{i,j,k}.vtime,    tempvdata] = ...
%				readVoltFile('default.vltg', expt.decimator, doEndianCheck, expectXFibers)
%
%   decimator = 10;
%
%	
% Description:
%	Reads the file written by CNERVE simulator
%
%
%
% INPUTS:
%	doEndianCheck	: (bool)		Should I try to read endian indicator at
%					  head of file?
%
%						DEFAULT:	read as IEEE-BE.
%
%
%	expectXFibers	: an optional value, useful for reading old format
%					  files which were written w/o an endian header, but
%					  were still run by different platforms. This variable
%					  will hint the function how many fibers to expect.
%					  If it doesn't get the # of fibers it expects, it'll
%					  reopen the file in w/a different endianness, and see
%					  how that works out.
%
%
%
% TODO: description
%
%
% Returns:
%	fibers			: (int), # of fibers read.  -1 if C program returned an error
%	fiblist			:
%	nodnums			: 
%	mcr				: (int), # of Monte Carlo simulations performed
%


fiblist = [];
nodnums = -1;
nstep	= -1;
E_rest	= -1;
delt	= -1;
time	= [];
voltages= [];


%%	CHECK AND SET DEFAULTS
%
if ~exist('VoltageSample', 'var')
	VoltageSample = 0.001;
else
	if VoltageSample < 0.001
		error('VoltageSample rate set to an invalid value: %d', VoltageSample);
	end
end
%	%	%	%	%	%	%	%	%	%	%	%	%	%	%	%	%	%	%	%	%	



%%	Determine which endian format the files were written in (or dont)
%
try
	if exist('doEndianCheck', 'var') && doEndianCheck

		endianFmt	= checkEndianness(filename);	% either 'ieee-le' or 'ieee-be' or error

		[fid, msg]	= fopen(filename, 'r', endianFmt);
		fread(fid, 1, 'uint32');					% skip the endianness indicator

	else
		[fid, msg]	= fopen(filename, 'r', 'ieee-be');
	end
catch e
	fclose(fid);
	rethrow(e);
end




try
	fibers	= fread(fid, 1, 'int32');
catch e
	fclose(fid);
	rethrow(e);
end


% if we know to expect a certain number of fibers, and we don't get it (most
% useful when reading in results from 'VIRGIN' program), reopen the file in
% another endianness, and see what we get:
if exist('expectXFibers', 'var') && fibers ~= expectXFibers
	%warning('resorting to exected fibers.');

	fclose(fid);
	fid		= fopen(filename, 'r', 'ieee-le');
	fibers	= fread(fid, 1, 'int32');
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
fiblist	= zeros(1,   fibers, 'int32');
fiblist	= fread(fid, fibers, 'int32=>int32')';


% maximum number of nodes for each fiber
nodnums	= zeros(1,	 fibers, 'int32');
nodnums	= fread(fid, fibers, 'int32=>int32')';

nstep	= fread(fid, 1, 'int32');
E_rest	= fread(fid, 1, 'double=>single');
delt	= fread(fid, 1, 'double=>single');
samples = round(nstep*delt/VoltageSample);

voltages = zeros(fibers,Montes,max(nodnums),samples);

for j=1:fibers
    for o=1:Montes
    for i=1:samples
        temp = E_rest+fread(fid, double(nodnums(j)),'double=>single');
        voltages(j,o,1:nodnums(j),i) = temp';
    end
    end
	clear temp;
end
fclose(fid);

time = 0:delt/VoltageSample:(nstep-1)*delt;
