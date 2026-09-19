# zusmt: Satisfiability Modulo Theories Solving with 'OpenSMT'

Solve satisfiability modulo theories (SMT) problems using the bundled
'OpenSMT' solver, described in Hyvärinen, Marescotti, Alt and Sharygina
(2016)
[doi:10.1007/978-3-319-40970-2_35](https://doi.org/10.1007/978-3-319-40970-2_35)
. Problems are written in the 'SMT-LIB' standard language and answers
come back as R values, with model values carrying the solver's exact
rational alongside the double. Supported theories include uninterpreted
functions, linear integer and real arithmetic and their combinations,
difference logic, and arrays. The solver is compiled into the package,
so no separately installed solver binary is required.

## See also

Useful links:

- <https://pedrobtz.github.io/zusmt/>

- Report bugs at <https://github.com/pedrobtz/zusmt/issues>

## Author

**Maintainer**: Pedro Baltazar <pedrobtz@gmail.com> \[copyright holder\]

Authors:

- Pedro Baltazar <pedrobtz@gmail.com> \[copyright holder\]

Other contributors:

- OpenSMT authors (bundled OpenSMT sources; see LICENSE.note)
  \[contributor, copyright holder\]
