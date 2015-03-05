
function x = basicVocoder(soundFile,P)
y = wavread(soundFile); %loading speech signal
Fs = 16000; %Sampling frequency
Fc = 4000;

N = 8;

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

[bLow,aLow] = fir1(N,2*(Fc/Fs));


for i = Fs*0.03:Fs*0.02:length(y)-0.025*Fs
    
    lastSpeech = i+1-0.015*Fs;
    nextSpeech = i+0.015*Fs;
    lastPitch = i+1-0.025*Fs;
    nextPitch = i+0.025*Fs;
    y_filt = windowSpeech.*y(lastSpeech:nextSpeech);
    y_pitch = windowPitch.*y(lastPitch:nextPitch);
    
    r_y = autocorr(y_filt);
    
    A = LevinsonDurbin(r_y,P);
    [pitchPeriod, voiced] = findPitchAndVoice(y_pitch,Fs); %Finding the period of the pitch and a the voiced constant
    
    if voiced >= alpha 
        last = i+0.01*Fs-pitchPeriod-1;
        for j = lastPulse:pitchPeriod:i+0.01*Fs
            pitch(j) = 1;
            if j> last
                lastPulse = j;
                
            end
        end
        
    else
        lastPulse = i+0.01*Fs;
        noise(i+1-0.01*Fs:i+0.01*Fs) = beta*randn(1,0.02*Fs);
        
    end
    vocoderInput = noise+pitch;
    vocoderInput(vocoderInput > 1) = 1;
    temp = vocoderInput(i+1-0.01*Fs:i+0.01*Fs);
    if i == Fs*0.03+49*Fs*0.02
        figure(8)
        plot(temp)
    end
    synthezised(i+1-0.01*Fs:i+0.01*Fs) = filter(1,A,temp);
    if i == Fs*0.03+49*Fs*0.02
        figure(9)
        plot(synthezised(i+1-0.01*Fs:i+0.01*Fs))
    end
    
end

synthezised = filter(bLow,aLow,synthezised);

x = synthezised;
end
