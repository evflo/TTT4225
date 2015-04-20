% Computing a Hamming window of length L given
% the formula for this window type: ham(n) = 0.54-0.46*cos(2*pi*(n/N)) 
% [Source: http://se.mathworks.com/help/signal/ref/hamming.html]
function ham = hammingWindow(L)
	n = 0:1:L-1;
	N = L-1;
	w = 0.54-0.46*cos(2*pi*(n/N));
	ham = w';
end %function