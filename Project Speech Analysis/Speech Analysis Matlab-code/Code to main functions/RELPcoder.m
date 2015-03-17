%The function RELPcoder takes a sound file and the order P to be used in the linear prediction as and
%returns to arrays called x_algorithm and x_upsample. x_algorithm is the output of high frequency regeneration
%using the algorithm explained in the task theory. x_upsample use upsampling to regenerate the
%high frequencies.
function [x_algorithm, x_upsample] = RELPcoder(soundFile,P)
	%Take in the data in the sound file
	speech = wavread(soundFile);
	
	%Initializing the signals that will be estimated
	synthetic_error = zeros(1,length(speech));
	synthetic_speech_upsampling = zeros(1,length(speech));
	synthetic_speech_HFregeneration = zeros(1,length(speech));
	
	%Declaring the theoretical values for different parts of the RELPcoder
	Fs = 16000; %Sampling frequency
	Fc = 2000; %Filter frequency (to be used in both lowpass- and highpassfilter)
	D = 4; %Decimated factor decided to be 4
	low_p = 4; %Order to be used in low p LP analysis of the refraction signal in high frequency regenerate
	N = 8; %Order to be used in the produced filters
	
	%Making a Hamming window, using a self-made function, used for estimation
	windowSpeech = hammingWindow(0.03*Fs); 
	
	%Finding the coefficients for both lowpass- and highpassfilter
	[bLow,aLow] = fir1(N,2*(Fc/Fs));
	[bHigh,aHigh] = fir1(N,2*(Fc/Fs),'high');

	%Indexing the for-loop to start after 0.03*Fs and then use 0.02*Fs as step
	for i = Fs*0.03:Fs*0.02:length(speech)-0.025*Fs
	
		%LastSpeech and nextSpeech keeps track of the start and end of the intervall to be processed with overlap-add
		lastSpeech = i+1-0.015*Fs;
		nextSpeech = i+0.015*Fs;
		
		%Filtrating the speech with the window function
		speech_filt = windowSpeech.*speech(lastSpeech:nextSpeech);
		
		%Finding the LP-coefficients using a made LevinsonDurbin function which uses the autocorrelation
		A = LevinsonDurbin(autocorr(speech_filt,P);
		
		%Filtrate the speech with found A-coefficients
		speech_est = filter(A,1,speech_filt);
		
		%Finding the difference between speech and LP-estimated data
		speech_error = speech_filt - speech_est;
		
		%Storing found result on synthetic_error which has the same length as input signal
		synthetic_error(i+1-0.01*Fs:i+0.01*Fs) = speech_error(0.005*Fs+1:0.025*Fs);
		
		%Lowpass filtering the error
		speech_error = filter(bLow,aLow,speech_error);
		
		%Decimation of filtrated error
		high_freq_regenerate  = decimate(speech_error,D);
		
		%Upsampling the decimated signal. This is the output to filtering in RELPcoder with upsampling the high frequency method
		zero_insertion = upsample(high_freq_regenerate,D); 

		%Dividing into low frequencies and high frequencies as described in the project theory
		LF_excitation= filter(bLow,aLow,zero_insertion);
		HF_excitation = LF_excitation;
		HF_excitation(HF_excitation<0 ) = 0; %Removing negative values of HF_excitation
		
		%Computing the low LP-analysis
		low_p_LP_analysis = LevinsonDurbin(autocorr(HF_excitation),low_p);
		HF_excitation = filter(low_p_LP_analysis,1,HF_excitation);
		
		%Higpass filtering HF_excitation to avoid higher frequencies
		HF_excitation = filter(bHigh,aHigh,HF_excitation);
		
		%Gain adjustment to adjust HF_excitation in relationship to the LF_excitation
		%with using the maximum of the auto correlation as gainfactor for both signals
		gainHF = max(autocorr(HF_excitation));
		gainLF = max(autocorr(LF_excitation));
		gain = gainLF/gainHF;
		HF_excitation = gain .* HF_excitation;
		
		%Addding both the low frequencies and the high frequencies 
		synthetic_fullband_residual = LF_excitation + HF_excitation
		;
		%Computing the upsampled solution
		speech_upsampling = filter(1,A,zero_insertion);
		
		%Computing the algorithm solution
		speech_HF = filter(1,A,synthetic_fullband_residual);
		
		%Storing the values
		synthetic_speech_upsampling(i+1-0.01*Fs:i+0.01*Fs) = speech_upsampling(0.005*Fs+1:0.025*Fs);
		synthetic_speech_HFregeneration(i+1-0.01*Fs:i+0.01*Fs) = speech_HF(0.005*Fs+1:0.025*Fs);
		
		
	end %for
	
	%Making the output for both methods
	x_algorithm = synthetic_speech_HFregeneration;
	x_upsample = synthetic_speech_upsampling;
	
	%Computing the Signal to noise - ratio for both methods and printing to screen
	r_a = snr(x_algorithm,speech'-x_algorithm);
	r_u = snr(x_upsample,speech'-x_upsample);
	fprintf('The SNR for upsampled method: %g \n The SNR for algorithm method: %g \n',r_u,r_a);

end %function