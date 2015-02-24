%Test for different P
Fs = 16000;
input = prompt('1 for testing Vocoder\n 2 for testing RELP \n 3 for testing both \n Push any other button for exit\n');

while 1==1
    switch input
        case 1
            testVocoder();
        case 2
            testRELP();
        case 3
            testVocoder();
            soundsc(1,Fs*10);
            testRELP();
        otherwise
            break;
    end
end

