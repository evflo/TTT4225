%Tar inn et kort signal x, med samplingsfrekvens Fs.
%Finner pitchens periode (pP) gitt i antall samples og ratioen (vR) mellom
%narmeste "harmoniske" og r_x(0). Ratioen gir ett m?l p? i hvilken grad
%lyden er stemt eller ikke.
function [pP, vR] = findPitchAndVoice(x, Fs)
    %r_x = xcorr(x,x);
    %r_x = r_x(ceil(length(r_x)/2):end);
    r_x = autocorr(x);
    pitchRange = floor(0.002*Fs):floor(0.02*Fs);
    pitchFrame = r_x(pitchRange);
    %figure;
    %plot(pitchFrame);
    
    minima = find(pitchFrame <= 0);
    if isempty(minima)
        minima = length(pitchFrame);
    end
    pitchPos = find(pitchFrame(minima(1):end) == max(pitchFrame(minima(1):end)));
    pP = pitchPos + minima(1) + pitchRange(1) - 3;
    vR = r_x(pP+1)/r_x(1);
    %figure;
    %plot(r_x);
end