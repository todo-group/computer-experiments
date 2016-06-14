PDF = lecture-1.pdf lecture-2.pdf lecture-3.pdf lecture-4.pdf lecture-5.pdf \
	lecture-6.pdf appendix.pdf \
	0_about.pdf 1_basics.pdf 2_ode.pdf 3_linear_system.pdf \
	4_eigenvalue_problem.pdf 5_linear_regression.pdf 6_monte_carlo.pdf \
	7_optimization.pdf

# PDF =  8_hpc.pdf 9_vcs.pdf

.SUFFIXES: .pdf .dvi .tex

default: $(PDF)

lecture-1.pdf: 00_about.tex 01_ssh.tex 10_error.tex 11_newton.tex
lecture-2.pdf: 20_ode.tex 21_symplectic.tex 90_vcs.tex 91_subversion.tex
lecture-3.pdf: 30_intro.tex 31_direct.tex 32_iterative.tex 13_lapack.tex
lecture-4.pdf: 40_intro.tex 41_dense.tex 42_sparse.tex 43_svd.tex
lecture-5.pdf: 50_least-square.tex 52_bayes.tex 60_randomized.tex 61_montecarlo_integration.tex 62_random_number.tex 63_mcmc.tex
lecture-6.pdf: 70_optimization.tex 71_enclosure.tex 72_newton.tex 73_descent.tex 74_simplex.tex 75_annealing.tex
appendix.pdf: 12_algebraic.tex 22_numerov.tex 09_exercise.tex 19_exercise.tex 29_exercise.tex 39_exercise.tex 49_exercise.tex 51_kernel.tex 59_exercise.tex 69_exercise.tex 

0_about.pdf: 00_about.tex 01_ssh.tex 09_exercise.tex
1_basics.pdf: 10_error.tex 11_newton.tex 12_algebraic.tex 13_lapack.tex 19_exercise.tex
2_ode.pdf: 20_ode.tex 21_symplectic.tex 22_numerov.tex 29_exercise.tex
3_linear_system.pdf: 30_intro.tex 31_direct.tex 32_iterative.tex 39_exercise.tex
4_eigenvalue_problem.pdf: 40_intro.tex 41_dense.tex 42_sparse.tex 43_svd.tex 49_exercise.tex
5_linear_regression.pdf: 50_least-square.tex 51_kernel.tex 52_bayes.tex 59_exercise.tex
6_monte_carlo.pdf: 60_randomized.tex 61_montecarlo_integration.tex 62_random_number.tex 63_mcmc.tex 69_exercise.tex
7_optimization.pdf: 70_optimization.tex 71_enclosure.tex 72_newton.tex 73_descent.tex 74_simplex.tex 75_annealing.tex 79_exercise.tex

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
