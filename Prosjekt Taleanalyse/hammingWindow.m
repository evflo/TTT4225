function ham = hammingWindow(L)
n = 0:1:L-1;
N = L-1;
w = 0.54-0.46*cos(2*pi*(n/N));
ham = w';
end