%Tar inn autokorrelasjon av et signal, r, og orden,p, p? 
%den line?re prediksjonen.
%Funksjon bruker Levinson og Durbins algoritme for ? finne
%filterkoeffisientene til signal som har autokorrelasjon r
%Returnerer filterkoeffisientene i arrayet A = [a(0) a(1) .. a(p)]

function A = LevinsonDurbin(r,p)

%r = r(length(r)/2:end); %Bruker kun ?vre del av korrelasjonen med energien gitt som r(0)
E = r(1); %Energien gitt i r(0)
b = [];
b(1) = 1; 

for i = 1:p
    k(i) = r(i+1);
    a = zeros(i+1,1);
    a(1) = 1;
    for j = 2:i
        k(i) = k(i)+b(j)*r(i-j+2);
    end %for j
    
    
    k(i) = -k(i)/E;
    for j = 2:i
        a(j) = b(j)+k(i)*b(i-j+2);
    end %for j
    a(i+1) = k(i);
    E = (1-abs(k(i))^2)*E;
    
    b = a;
end %for i
A = a';

end %function
    
