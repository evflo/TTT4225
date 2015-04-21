% Takes in a short signal x, with a given sampling frequency Fs
% Finds the pitch periode (pP) given as the number of samples and the ratio (vR) 
% between the closest "harmonic" and r_x(0)
% The ratio indecates if the signal is voiced or not
function [pP, vR] = findPitchAndVoice(x, Fs)
	%Finds the auto correlation of x
    r_x = autocorr(x);
	
	% Define the pitch properties for range and frame
    pitchRange = floor(0.002*Fs):floor(0.02*Fs);
    pitchFrame = r_x(pitchRange);
   
    % Finds where the frame is less then zeros or sets minima as the length of pitchFrame
	% if no minima is found
    minima = find(pitchFrame <= 0);
    if isempty(minima)
        minima = length(pitchFrame);
    end %if
	
	% Finds the pitch positions
    pitchPos = find(pitchFrame(minima(1):end) == max(pitchFrame(minima(1):end)));
	
	% Sets the value of output values by computed data
    pP = pitchPos + minima(1) + pitchRange(1) - 3;
    vR = r_x(pP+1)/r_x(1);
    
end %function