%Auto correlation function which is easier to implement in C
%Finds the auto correlation with the signal energy r(0) as starting point
function r = autocorr(x)
	x = x';
	N = length(x);
	x = [x,zeros(1,N+1)];
	%Stores the auto correlation values in C in the iterations
	C = zeros(1,N);

	%Compute the auto correlation using the formula definiton of correlation
	for k = 1:N
		for n = 1:N
			C(k) = C(k)+ x(n)*x(n+k-1);
		end %for n
	end %for k

	%Copies the wanted area of C to the auto correlation output
	r = C(1:N);
	r = r';

end %function