%testVocoder is a test function for basicVocoder for different values of P
%to be used to answer the group specific task.
%The function plays the output of basicVocoder for randomly picked P
%to make the review of quality be as precise as possible
function testVocoder()

	%Different P-values to be tested
    P = [2 4 6 8 10 20]; 
	
<<<<<<< Updated upstream
    Fs = 16000; %Sampling frequency
    pause = zeros(1,10*Fs); %10 sec pause signal which gives time to subjectively review the signal
	
=======
    Fs = 16000; %Sampling frequency	
>>>>>>> Stashed changes
	%First playing the original signal
    fprintf('Play of original\n');
    soundsc(basicVocoder('anvsb1.wav',14),Fs);
    pause();
    while ~isempty(P)
		%Randomly choosen value of P to be played
        k = ceil(rand*length(P));
		
		%Find output for a random P
        x = basicVocoder('anvsb1.wav',P(k)); 
		
		%Play of output for choosen P followed by a pause
        fprintf('Output for P = %d is now being played \n',P(k)); 
        soundsc(x,Fs); 
<<<<<<< Updated upstream
        soundsc(pause,Fs);

=======
        pause();
>>>>>>> Stashed changes
		%Plot of the signal is also presented to be used in review
        figure;
		text = sprintf('Plot for P = %d',P(k));
        plot(x),title(text);
		
		%Discard P-values that have been tested
        P(k) = []; 

    end %while
<<<<<<< Updated upstream
end %function
=======
end %function
>>>>>>> Stashed changes
