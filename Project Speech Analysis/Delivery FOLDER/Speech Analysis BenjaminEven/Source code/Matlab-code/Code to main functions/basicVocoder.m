% Reads audio from a .wav-file specified in the string soundFile, analyze the signal properties
% and regenerate the signal using P filter coefficients.

function x = basicVocoder(soundFile,P)
y = wavread(soundFile); %loading speech signal
Fs = 16000; %Sampling frequency
Fc = 4000; %Cut-off frequency for low-pass filter
N = 8; % Filter order for FIR low-pass filter

alpha = 0.5; %Boundary for what will be seen as voiced
beta= 0.1; %Gain to the noise signal

windowSpeech = hammingWindow(0.03*Fs);
windowPitch = hammingWindow(0.05*Fs);
%Initialize the arrays to be used in the basic vocoder for synthetic speech
pitch = zeros(1,length(y));
noise = zeros(1,length(y));
vocoderInput = zeros(1,length(y));
synthezised = zeros(1,length(y));
%Initialize the last values
last = 1; 
lastPulse = 1;

% Find filter coefficients for a N'th order FIR low-pass filter with cut-off frequency Fc.
[bLow,aLow] = fir1(N,2*(Fc/Fs));

% Iterating through the signal, analyzing and regenerating the signal 20ms at a time.
% 'i' marks the middle of the 20ms frame.
for i = Fs*0.03:Fs*0.02:length(y)-0.025*Fs
    % Shape the relevant signal segments using Hamming-windows.
    lastSpeech = i+1-0.015*Fs;
    nextSpeech = i+0.015*Fs;
    lastPitch = i+1-0.025*Fs;
    nextPitch = i+0.025*Fs;
    y_filt = windowSpeech.*y(lastSpeech:nextSpeech);
    y_pitch = windowPitch.*y(lastPitch:nextPitch);

    r_y = autocorr(y_filt);
    % Levinson-Durbin recursion is used on the auto-correlation of the signal to find filter coefficients
    A = LevinsonDurbin(r_y,P);
    % Find the period of the pitch and a voiced constant (between 0 and 1, larger for voiced sounds)
    [pitchPeriod, voiced] = findPitchAndVoice(y_pitch,Fs); 
    
    % Generate a signal using either white noise or pulse trains depending on the voiced properties
    % of the signal segment.
    if voiced >= alpha 
        % When the signal is voiced, make a pulse train of "dirac"-pulses with intervals
        % defined by the previously found pitch period.
        last = i+0.01*Fs-pitchPeriod-1;
        for j = lastPulse:pitchPeriod:i+0.01*Fs
            pitch(j) = 1;
            if j> last
                lastPulse = j; 
            end
        end
    else
        % When the signal isn't voiced, white noise is used as a signal, scaled down with 'beta' gain.
        lastPulse = i+0.01*Fs;
        noise(i+1-0.01*Fs:i+0.01*Fs) = beta*randn(1,0.02*Fs);
    end
    vocoderInput = noise+pitch;
    
    % Adjusts numbers that are too large, in the rare case of white noise getting very large values.
    vocoderInput(vocoderInput > 1) = 1;
    temp = vocoderInput(i+1-0.01*Fs:i+0.01*Fs);
    % Filter the generated signal using the filter coefficients found by the Levinson-Durbin recursion.
    synthezised(i+1-0.01*Fs:i+0.01*Fs) = filter(1,A,temp);
end
% Low-pass filter the generated signal to remove spurious high frequency elements.
synthezised = filter(bLow,aLow,synthezised);

x = synthezised;
end
