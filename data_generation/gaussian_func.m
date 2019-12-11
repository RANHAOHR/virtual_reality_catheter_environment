function res=gaussian_func(dist,sigma)

res = 1/ (sqrt(2* pi) * sigma) * exp(-( dist ) / (2 * sigma * sigma)) * 10;

end