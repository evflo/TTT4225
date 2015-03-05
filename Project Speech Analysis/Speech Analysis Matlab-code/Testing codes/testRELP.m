function testRELP()

    P = [2 4 6 8 10 20]; %Different P
    Fs = 16000; %Sampling frequency
    pause = zeros(1,10*Fs); %Have pause signal which gives time to subjectively review the signal

    fprintf('Play of original\n');
    soundsc(RELP('anvsb1.wav',14),Fs);
    soundsc(pause,Fs)

    while ~isempty(P)
        k = ceil(rand*length(P));
        x = RELPcoder('anvsb1.wav',P(k));

        text = sprintf('Plot for P = %d',P(k));

        fprintf('Output for P = %d is now being played \n',P(k));

        soundsc(x,Fs);
        soundsc(pause,Fs);


        figure;
        plot(x),title(text);
        P(k) = [];

    end
    
end