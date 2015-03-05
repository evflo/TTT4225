function testVocoder()

    P = [2 4 6 8 10 20]; %Different P
    Fs = 16000; %Sampling frequency
    pause = zeros(1,10*Fs); %Have pause signal which gives time to subjectively review the signal

    fprintf('Play of original\n');
    soundsc(basicVocoder('anvsb1.wav',14),Fs);
    soundsc(pause,Fs)
    while ~isempty(P)
        k = ceil(rand*length(P)); %Find a random P
        x = basicVocoder('anvsb1.wav',P(k)); %Find output for a random P

        text = sprintf('Plot for P = %d',P(k));

        fprintf('Output for P = %d is now being played \n',P(k)); 

        soundsc(x,Fs); %Play of output for choosen P
        soundsc(pause,Fs);


        figure;
        plot(x),title(text); %Plot for choosen P
        P(k) = []; %Discard P-values that have been tested

    end
end
