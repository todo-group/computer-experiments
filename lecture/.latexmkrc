#!/usr/bin/env perl
$latex            = 'TEXINPUTS=.:./style//:./1_basics//:./2_ode//:./3_linear_system//:./4_eigenvalue_problem//: platex -synctex=1 -halt-on-error';
$latex_silent     = 'TEXINPUTS=.:./style//:./1_basics//:./2_ode//:./3_linear_system//:./4_eigenvalue_problem//: platex -synctex=1 -halt-on-error -interaction=batchmode';
$bibtex           = 'pbibtex';
$dvipdf           = 'dvipdfmx %O -o %D %S';
$makeindex        = 'mendex %O -o %D %S';
$max_repeat       = 5;
$pdf_mode         = 3;
$pvc_view_file_via_temporary = 0;
