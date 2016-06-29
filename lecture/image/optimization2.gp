set xrange [-4:4]
set yrange [-4:4]
set size square
set grid
unset key
plot \
"mc_steepest_descent_2d.dat" u 2:3 title "Steepest Descent (MC)" w l ,\
"simulated_annealing_2d.dat" u 3:4 title "Simulated Annealing" w l ,\
"steepest_descent_2d.dat" u 2:3 title "Steepest Descent" w l lw 2,\
"gradient_descent_2d.dat" u 2:3 title "Gradient Descent" w l lw 2,\
"cg_2d.dat" u 2:3 title "Conjugate Gradient" w l lw 2,\
"nelder_mead_2d.dat" u 4:5 title "Nelder Mead" w l
