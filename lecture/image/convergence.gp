set size square
set grid
set log
set yrange [0.000001:100]
set ytics ("10^{-6}" 0.000001, "" 0.00001, "10^{-4}" 0.0001, "" 0.001, "10^{-2}" 0.01, "" 0.1, "10^{0}" 1.0, "" 10.0, "10^{2}" 100.0)
set xlabel 'iteration'
set ylabel 'distance'
set nokey
plot \
"mc_steepest_descent_2d.dat" u 1:5 title "Steepest Descent (MC)" w l ,\
"simulated_annealing_2d.dat" u 1:6 title "Simulated Annealing" w l ,\
"steepest_descent_2d.dat" u 1:5 title "Steepest Descent" w l lw 2,\
"gradient_descent_2d.dat" u 1:5 title "Gradient Descent" w l lw 2,\
"cg_2d.dat" u 1:5 title "Conjugate Gradient" w l lw 2,\
"nelder_mead_2d.dat" u ($0):8 title "Nelder Mead" w l
