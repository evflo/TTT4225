%Autokorrelasjonsfunksjon som kan lettere implementeres i C
%Lager tosidig spekter med r(0) som middelpunkt
%(Kan endres til ? bli ensidig med r(0)-energien som startpunkt)
%Tilsvarer n? xcorr-funksjonen
function r = autocorr(x)
x = x';
N = length(x);
x = [x,zeros(1,N+1)];
C = zeros(1,N); %Lagrer autokorrelasjonsverdiene i C i iterasjonene
for k = 1:N
    for n = 1:N
        C(k) = C(k)+ x(n)*x(n+k-1);
        
    end
end
%F?lgende kan sl?yfes og erstattes med r = C; for ensidig spekter med energien som
%startverdi
%r = zeros(1,2*length(x)-1); 
%r((length(C)+1):(length(r))-1) = C;
%for i = 1:length(C)
%    r(length(C)-i+1) = C(i);
%end
r = C(1:N);
r = r';
end