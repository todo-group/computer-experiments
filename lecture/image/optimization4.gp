set xrange [1.61:1.65]
set yrange [1.61:1.65]
set size square
set grid
plot \
"steepest_descent_2d.dat" u 2:3 title "Steepest Descent" w l lw 2,\
"gradient_descent_2d.dat" u 2:3 title "Gradient Descent" w l lw 2,\
"cg_2d.dat" u 2:3 title "Conjugate Gradient" w l lw 2
