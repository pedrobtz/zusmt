#ifndef ZUSMT_DIFFERENCE_LOGIC_H
#define ZUSMT_DIFFERENCE_LOGIC_H

// What OpenSMT's difference-logic solver can read.
//
// QF_IDL and QF_RDL are decided by STPSolver, whose parseRef() assumes every
// atom has already been normalised to
//
//     (<= c v)    (<= c (* -1 w))    (<= c (+ v (* -1 w)))
//
// -- a constant on the left, and on the right at most one variable with
// coefficient 1 and at most one with coefficient -1. It checks that only with
// assert(), which R compiles out (-DNDEBUG), and on anything else it reads a
// coefficient and a variable out of whatever the term table holds there. So
// (< (+ x y) z) under QF_IDL used to come back "sat" with a model violating
// it, and some satisfiable problems came back "unsat".
//
// This predicate is that shape, written down once and used twice:
//
//   * by src/solver.cc, before an assertion reaches the solver, so input
//     outside the logic is refused with an error naming the atom; and
//   * by the vendored parseRef() itself (patch rule 13 in tools/patches.sh),
//     so an atom that somehow got past the first check is an error rather
//     than a wrong answer.
//
// `before_preprocessing` is the difference between the two uses. An
// assertion is checked as parsed, when an arithmetic `ite` still stands where
// the solver will later see a fresh variable; parseRef() runs after that
// replacement and sees only variables.

#include <logics/ArithLogic.h>

namespace zusmt {

inline bool dl_variable(opensmt::ArithLogic const & logic, opensmt::PTRef t,
                        bool before_preprocessing) {
    if (logic.isNumVar(t)) return true;
    return before_preprocessing && logic.isIte(t) && logic.isSortNum(logic.getSortRef(t));
}

// (* -1 w): the only product parseRef() accepts, constant first.
inline bool dl_negated_variable(opensmt::ArithLogic const & logic, opensmt::PTRef t,
                                bool before_preprocessing) {
    if (!logic.isTimes(t)) return false;
    opensmt::Pterm const & times = logic.getPterm(t);
    return times.size() == 2 && logic.isNumConst(times[0]) &&
           logic.getNumConst(times[0]) == -1 &&
           dl_variable(logic, times[1], before_preprocessing);
}

// The right-hand side of a normalised difference atom.
inline bool dl_difference_term(opensmt::ArithLogic const & logic, opensmt::PTRef t,
                               bool before_preprocessing) {
    if (dl_variable(logic, t, before_preprocessing)) return true;
    if (dl_negated_variable(logic, t, before_preprocessing)) return true;
    if (!logic.isPlus(t)) return false;
    opensmt::Pterm const & plus = logic.getPterm(t);
    if (plus.size() != 2) return false;
    return (dl_variable(logic, plus[0], before_preprocessing) &&
            dl_negated_variable(logic, plus[1], before_preprocessing)) ||
           (dl_variable(logic, plus[1], before_preprocessing) &&
            dl_negated_variable(logic, plus[0], before_preprocessing));
}

// (<= c t), with t as above: ArithLogic builds every inequality with the
// constant first, which is what parseRef() relies on.
inline bool dl_inequality(opensmt::ArithLogic const & logic, opensmt::PTRef atom,
                          bool before_preprocessing) {
    opensmt::Pterm const & leq = logic.getPterm(atom);
    return leq.size() == 2 && logic.isNumConst(leq[0]) &&
           dl_difference_term(logic, leq[1], before_preprocessing);
}

// (= c t). The equality rewriter later splits it into (<= c t) and
// (<= -c -t), and negating a difference keeps it one. Either argument order
// is accepted: equality is commutative and the logic may store it sorted.
inline bool dl_equality(opensmt::ArithLogic const & logic, opensmt::PTRef atom,
                        bool before_preprocessing) {
    opensmt::Pterm const & eq = logic.getPterm(atom);
    if (eq.size() != 2) return false;
    return (logic.isNumConst(eq[0]) && dl_difference_term(logic, eq[1], before_preprocessing)) ||
           (logic.isNumConst(eq[1]) && dl_difference_term(logic, eq[0], before_preprocessing));
}

}  // namespace zusmt

#endif  // ZUSMT_DIFFERENCE_LOGIC_H
