function parameters = binompdftoparams(varargin)
% Sample usage:
%   cutoffs = binompdftoparams('fibnum',fibnum, 'N', cutMax, 'prob',...
%   cutProb, 'min', cutMin, 'max', cutMax)
% 
% Description: This function creates a list of parameter values drawn
% randomly from a binomial distribution with 'N' trials and 'prob' per
% trial of length 'fibnum'. 'min' and 'max' can be used to truncate values
% out of the desired range to the most extreme permissible values.
%
% Author:   Jesse (2017)

% Using this code to set the probabilities enables conisistent sampling of
% the entire range. Not sure that's necessary with the population sizes
% though.
% paramprobs =0.5/(fibnum):1/(fibnum):1-0.5/(fibnum);
% paramprobs = paramprobs(randperm(fibnum));

p = inputParser;
p.addParameter('fibnum','1', @(x)x>0 && mod(x,1)==0);
p.addParameter('N','0', @(x)x>0 && mod(x,1)==0);
p.addParameter('prob','0', @(x)x>0);
p.addParameter('min','[]', @(x)x>=0 && mod(x,1)==0);
p.addParameter('max',32, @(x)x>0 && mod(x,1)==0);
p.addOptional('PrintPDF','No-Print',@(x)any(strcmpi(x,{'No-Print','Print'})));
p.parse(varargin{:});

paramprobs = rand(1,p.Results.fibnum);

pd = makedist('Binomial','N',p.Results.N,'p',p.Results.prob);

% One alternative pdf to use is an inverse Gaussian. They match some of the
% structural data more closely but the parameters are harder to guess.
% pd = makedist('InverseGaussian','Mu',2e-3,'Lambda',.01);
% parameters = icdf(igauss,paramprobs);

parameters = icdf(pd,paramprobs);

% If some values will fall out of the desired range, min and max can be
% used to constrain them.
if isinteger(p.Results.min) 
    if p.Results.min > p.Results.N
        parameters(parameters < p.Results.min) = p.Results.min;
    end
end

if isinteger(p.Results.max)
    if p.Results.max < p.Results.N
        parameters(parameters > p.Results.max) = p.Results.max;
    end
end
if strcmpi(p.Results.PrintPDF,'Print')
    histogram(parameters); hold on
end
end