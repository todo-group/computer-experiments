f(x) = (5*x+x**2)+70*sin(x)
set samples 1000
set xlabel 'x'
set ylabel 'Boltzmann weight (arbitrary unit.)'
plot [-20:20] exp(-f(x)/1000) title 'T = 1000', exp(-f(x)/100) title 'T=100', exp(-f(x)/10)/300 title 'T=10'
