PDF = lecture-1-1.pdf lecture-1-2.pdf lecture-1-3.pdf lecture-1-4.pdf \
	lecture-2-1.pdf \
	appendix.pdf \
	0_about.pdf 1_basics.pdf 2_ode.pdf 3_linear_system.pdf \
	4_eigenvalue_problem.pdf 5_linear_regression.pdf 6_monte_carlo.pdf \
	7_optimization.pdf 8_hpc.pdf

# PDF =  8_hpc.pdf 9_vcs.pdf

.SUFFIXES: .pdf .dvi .tex

default: $(PDF)

lecture-1-1.pdf: 00_about1.tex 02_ssh.tex 10_error.tex 11_newton.tex
lecture-1-2.pdf: 20_ode.tex 22_numerov.tex 21_symplectic.tex 30_intro.tex 31_direct.tex
lecture-1-3.pdf: 13_lapack.tex 32_iterative.tex 40_intro.tex 41_dense.tex
lecture-1-4.pdf: 42_sparse.tex 43_svd.tex 50_least-square.tex

lecture-2-1.pdf: 01_about2.tex 44_double-well.tex

# lecture-6.pdf: 70_optimization.tex 11_newton.tex 71_enclosure.tex 72_descent.tex 73_cg.tex 74_simplex.tex 75_annealing.tex 76_comparison.tex
# lecture-7.pdf: 70_optimization.tex 72_descent.tex 76_comparison.tex 80_supercomputer.tex	81_parallel.tex 82_batch.tex
# appendix.pdf: 12_algebraic.tex 22_numerov.tex 09_exercise.tex 19_exercise.tex 29_exercise.tex 39_exercise.tex 49_exercise.tex 51_kernel.tex 59_exercise.tex 69_exercise.tex 79_exercise.tex 89_exercise.tex

0_about.pdf: 00_about1.tex 01_about2.tex 02_ssh.tex 09_exercise.tex
1_basics.pdf: 10_error.tex 11_newton.tex 12_algebraic.tex 13_lapack.tex 14_bisection.tex 19_exercise.tex
2_ode.pdf: 20_ode.tex 21_symplectic.tex 22_numerov.tex 29_exercise.tex
3_linear_system.pdf: 30_intro.tex 31_direct.tex 32_iterative.tex 39_exercise.tex
4_eigenvalue_problem.pdf: 40_intro.tex 41_dense.tex 42_sparse.tex 43_svd.tex 44_double-well.tex 49_exercise.tex
5_linear_regression.pdf: 50_least-square.tex 51_kernel.tex 52_bayes.tex 59_exercise.tex
6_monte_carlo.pdf: 60_randomized.tex 61_montecarlo_integration.tex 62_random_number.tex 63_mcmc.tex 69_exercise.tex
7_optimization.pdf: 70_optimization.tex 11_newton.tex 71_enclosure.tex 72_descent.tex 73_cg.tex 74_simplex.tex 75_annealing.tex 76_comparison.tex 79_exercise.tex
8_hpc.pdf: 80_supercomputer.tex	81_parallel.tex 82_batch.tex 89_exercise.tex

.tex.dvi:
	TEXINPUTS=.:./style//: platex -shell-escape $<
	TEXINPUTS=.:./style//: platex $<
	TEXINPUTS=.:./style//: platex $<
.dvi.pdf:
	TEXINPUTS=.:./style//: dvipdfmx -I 24 $<

clean:
	rm -f *.aux *.dvi *.log *.nav *.out *.snm *.toc *.vrb
	rm -rf auto *.xbb ./image/*.xbb ./style/*.xbb

distclean: clean
	rm -f $(PDF)
