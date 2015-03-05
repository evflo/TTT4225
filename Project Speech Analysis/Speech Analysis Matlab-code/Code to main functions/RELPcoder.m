function [x_algorithm, x_upsample] = RELPcoder(soundFile,P)

speech = wavread(soundFile);
figure(19);
plot(speech);

%Initialize the signals that will be estimated
synthetic_error = zeros(1,length(speech));
synthetic_speech_upsampling = zeros(1,length(speech));
synthetic_speech_HFregeneration = zeros(1,length(speech));


Fs = 16000; %Sampling frequency
Fc = 2000; %Filter frequency
D = 4; %Decimated factor decided to be 4
low_p = 4; %Order to be used in low p LP analysis of refrection signal in high frequency regenerate
N = 8; %Order to be used in the produced LowPass filter

windowSpeech = hammingWindow(0.03*Fs); %Hamming window used for estimation

[bLow,aLow] = fir1(N,2*(Fc/Fs));
[bHigh,aHigh] = fir1(N,2*(Fc/Fs),'high');

figure(17);
freqz(bLow,aLow);
figure(18);
freqz(bHigh,aHigh);

for i = Fs*0.03:Fs*0.02:length(speech)-0.025*Fs
    
    lastSpeech = i+1-0.015*Fs;
    nextSpeech = i+0.015*Fs;

    speech_filt = windowSpeech.*speech(lastSpeech:nextSpeech);
    
    r_y = autocorr(speech_filt);
    
    A = LevinsonDurbin(r_y,P);
    
    speech_est = filter(A,1,speech_filt);
    
    speech_error = speech_filt - speech_est;
    
    synthetic_error(i+1-0.01*Fs:i+0.01*Fs) = speech_error(0.005*Fs+1:0.025*Fs);
    
    speech_error = filter(bLow,aLow,speech_error);
    
    high_freq_regenerate  = decimate(speech_error,D);
    
    zero_insertion = upsample(high_freq_regenerate,D); %To be used for both versions

    
    LF_excitation= filter(bLow,aLow,zero_insertion);
    
    HF_excitation = LF_excitation;
    
    
    HF_excitation(HF_excitation<0 ) = 0;
            
    low_p_LP_analysis = LevinsonDurbin(autocorr(HF_excitation),low_p);
    
    HF_excitation = filter(low_p_LP_analysis,1,HF_excitation);
    
    HF_excitation = filter(bHigh,aHigh,HF_excitation);
    gainHF = max(autocorr(HF_excitation));
    gainLF = max(autocorr(LF_excitation));
    gain = gainLF/gainHF;
    
    
    HF_excitation = gain .* HF_excitation;
    
    synthetic_fullband_residual = LF_excitation + HF_excitation;
    
    speech_upsampling = filter(1,A,zero_insertion);
    speech_HF = filter(1,A,synthetic_fullband_residual);
    synthetic_speech_upsampling(i+1-0.01*Fs:i+0.01*Fs) = speech_upsampling(0.005*Fs+1:0.025*Fs);
    synthetic_speech_HFregeneration(i+1-0.01*Fs:i+0.01*Fs) = speech_HF(0.005*Fs+1:0.025*Fs);
    
    
end
x_algorithm = synthetic_speech_HFregeneration;
x_upsample = synthetic_speech_upsampling;
figure(14);
plot(x_algorithm);
r_a = snr(x_algorithm,speech'-x_algorithm)
r_u = snr(x_upsample,speech'-x_upsample)
end