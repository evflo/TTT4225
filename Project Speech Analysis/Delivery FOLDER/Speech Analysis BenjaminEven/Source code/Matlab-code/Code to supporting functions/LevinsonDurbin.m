% The function LevinsonDurbin takes the auto correlation of a signal and the linear prediciton order P
% as input, and returns an array of the different coefficients for the wanted value of P
function A = LevinsonDurbin(r,p)

	%Initializes error power stored in r(0) (Matlab 1-indexed)
	E = r(1);

	%The array is to be used to produce A and is initalized and the first value of b is 1
	b = [];
	b(1) = 1; 

	for i = 1:p
		%Calculating the i'th reflection coefficient
		k(i) = r(i+1); 
		
		%Initializes a every iteration and manually sets value of a(1) = 1
		a = zeros(i+1,1);
		a(1) = 1;
		
		%Computing the i'th order predicitor coefficients
		for j = 2:i
			k(i) = k(i)+b(j)*r(i-j+2);
		end %for j
		
		%Adjusting k by the prediction error power
		k(i) = -k(i)/E;
		
		%Computing a for found b and k
		for j = 2:i
			a(j) = b(j)+k(i)*b(i-j+2);
		end %for j
		
		a(i+1) = k(i);
		%Prediction error power for i'th power
		E = (1-abs(k(i))^2)*E;
		%Copying the values of a in to the temporary array b 
		b = a;
		
	end %for i

	%Setting a to the output array A
	A = a';

end %function
