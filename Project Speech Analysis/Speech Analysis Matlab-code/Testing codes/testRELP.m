% testRELP is a test function for RELPcoder for different values of P
% to be used to answer the group specific task.
% The function plays the output of RELPcoder for randomly picked P
% to make the review of quality be as precise as possible
function testRELP()
	
	%Different P-values to be tested
    P = [2 4 6 8 10 20];
	
    Fs = 16000; %Sampling frequency

	%First playing the original signal
    fprintf('Play of original\n');
    [x_alg x_up] = RELPcoder('anvsb1.wav',14);
    soundsc(x_alg,Fs);
    pause();
    while ~isempty(P)
		%Randomly choosen value of P to be played
        k = ceil(rand*length(P));
		
		%Find output for a random P
        [x_alg x_up] = RELPcoder('anvsb1.wav',P(k));
		
		%Play of output for choosen P followed by a pause
        fprintf('Output for P = %d is now being played \n',P(k));
        soundsc(x_alg,Fs);
        pause();
		%Plot of the signal is also presented to be used in review
        figure;
        text = sprintf('Plot for P = %d',P(k));
        plot(x_alg),title(text);
       	
		%Discard P-values that have been tested
		P(k) = [];

    end %while
end %function