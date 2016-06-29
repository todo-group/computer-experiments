set xrange [-4:4]
set yrange [-4:4]
set size square
set grid
set key bottom
plot \
"steepest_descent_2d.dat" u 2:3 title "Steepest Descent" w l lw 2,\
"gradient_descent_2d.dat" u 2:3 title "Gradient Descent" w l lw 2,\
"cg_2d.dat" u 2:3 title "Conjugate Gradient" w l lw 2
