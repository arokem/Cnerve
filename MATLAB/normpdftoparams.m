function parameters = normpdftoparams(varargin)
% Sample usage:
%   diameters = normpdftoparams('fibnum',fibnum, 'avg',... diamMean, 'std',
%   diamStdev, 'min', minDiam, 'max', maxDiam)
%
% Description: This function creates a list of parameter values drawn
% randomly from a normal distribution with mu ('avg') and sigma ('std') of
% length 'fibnum'. 'min' and 'max' can be used to truncate values out of
% the desired range to the most extreme permissible values. May someday
% create a switch allowing use of 'InverseGaussian' pdf objects.
%
% Author:   Jesse (2017)

% Using this code to set the probabilities enables conisistent sampling of
% the entire range. Not sure that's necessary with the population sizes
% though. paramprobs =0.5/(fibnum):1/(fibnum):1-0.5/(fibnum); paramprobs =
% paramprobs(randperm(fibnum));

p = inputParser;
p.addParameter('fibnum',1, @(x)x>0 && mod(x,1)==0);
p.addParameter('avg',2e-3, @(x)x>=0);
p.addParameter('std',5e-4, @(x)x>0);
p.addParameter('min',0, @(x)x>=0);
p.addParameter('max','', @(x)x>0);
p.addOptional('PrintPDF','No-Print',@(x)any(strcmpi(x,{'No-Print','Print'})));
p.parse(varargin{:});

if ~isempty(p.Results.min)
    min = p.Results.min;
else
    min = -inf;
end
if ~isempty(p.Results.max)
    max = p.Results.max;
else
    max = inf;
end


% Create initial draw of random numbers.
paramprobs = rand(1,p.Results.fibnum);

% Define parameter probability distribution function to draw from.
pd = makedist('Normal','mu',p.Results.avg,'sigma',p.Results.std);
% One alternative pdf to use is an inverse Gaussian. They match some of the
% structural data more closely but the parameters are harder to guess. pd =
% makedist('InverseGaussian','Mu',2e-3,'Lambda',.01); parameters =
% icdf(igauss,paramprobs);

% Make first pass batch of paramter values.
parameters = icdf(pd,paramprobs);

% Re-roll values outside of the user-defined range. Note: it the variance
% of the pdf pushes most values out of range this could take a while to
% execute...just sayin.
while any(parameters < min | parameters > max)
    bad = parameters < min | parameters > max;
    fibsToMake = sum(bad);
    paramprobs(bad) = rand(1,fibsToMake);
    parameters = icdf(pd,paramprobs);
end

end