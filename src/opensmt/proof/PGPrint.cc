#include <r_compat.h>
/*
 *  Copyright (c) 2013, Simone Fulvio Rollini <simone.rollini@gmail.com>
 *
 *  SPDX-License-Identifier: MIT
 *
 */

#include "PG.h"

namespace opensmt {

// Prints resolution proof graph to a dot file,
// with proper colors
void ProofGraph::printProofAsDotty(std::ostream & out) {
    // Visited nodes vector
    std::vector<bool> visited_dotty(getGraphSize(), false);
    // Visit proof graph from sink via DFS
    std::vector<ProofNode *> q;
    q.push_back(getRoot());

    out << "digraph proof {" << '\n';

    while (!q.empty()) {
        ProofNode * node = q.back();
        // Remove node
        q.pop_back();
        // Node not yet visited
        if (!visited_dotty.at(node->getId())) {
            //Clean
            //clauseSort(node->clause);
            // Print node
            std::string typ;
            std::string color = "";
            switch (node->getType()) {
                case clause_type::CLA_ORIG:
                    typ = "cls_";
                    out << typ << node->getId() << "[shape=plaintext, label=\"c" << node->getId() << "  :  ";
                    printClause(node, out);
                    out << "\", color=\"lightblue\",";
                    out << " fontcolor=\"black\", style=\"filled\"]" << '\n';
                    break;
                case clause_type::CLA_THEORY:
                    typ = "the_";
                    out << typ << node->getId() << "[shape=plaintext, label=\"c" << node->getId() << "  :  ";
                    if (!node->getClause().empty()) { printClause(node, out); }
                    else out << "+"; // learnt clause
                    out << "\", color=\"grey\"";
                    out << ", style=\"filled\"]" << '\n';
                    break;
                case clause_type::CLA_LEARNT:
                    typ = "lea_";
                    out << typ << node->getId() << "[shape=plaintext, label=\"c" << node->getId() << "  :  ";
                    if (!node->getClause().empty()) { printClause(node, out); }
                    else out << "+"; // learnt clause
                    out << "\", color=\"orange\"";
                    out << ", style=\"filled\"]" << '\n';
                    break;
                case clause_type::CLA_DERIVED:
                    typ = "der_";
                    out << typ << node->getId() << "[shape=plaintext, label=\"c" << node->getId() << "  :  ";
                    if (!node->getClause().empty()) { printClause(node, out); }
                    else out << "+"; // internal derived clause
                    out << "\", color=\"green\"";
                    out << ", style=\"filled\"]" << '\n';
                    break;
                case clause_type::CLA_ASSUMPTION:
                    typ = "ass_";
                    out << typ << node->getId() << "[shape=plaintext, label=\"c" << node->getId() << "  :  ";
                    assert(!node->getClause().empty());
                    printClause(node, out);
                    out << "\", color=\"yellow\"";
                    out << ", style=\"filled\"]" << '\n';
                    break;
                case clause_type::CLA_SPLIT:
                    typ = "spl_";
                    out << typ << node->getId() << "[shape=plaintext, label=\"c" << node->getId() << "  :  ";
                    assert(not node->getClause().empty());
                    printClause(node, out);
                    out << "\", color=\"purple\"";
                    out << ", style=\"filled\"]" << '\n';
                    break;
                default:
                    assert(false);
                    typ = "";
                    break;
            }

            // Print edges from parents (if existing)
            auto addEdgeToParent = [&out, &typ](const ProofNode & parent, const ProofNode & child) {
                std::string t1;
                switch (parent.getType()) {
                    case clause_type::CLA_ORIG:
                        t1 = "cls_";
                        break;
                    case clause_type::CLA_THEORY:
                        t1 = "the_";
                        break;
                    case clause_type::CLA_LEARNT:
                        t1 = "lea_";
                        break;
                    case clause_type::CLA_DERIVED:
                        t1 = "der_";
                        break;
                    case clause_type::CLA_ASSUMPTION:
                        t1 = "ass_";
                        break;
                    case clause_type::CLA_SPLIT:
                        t1 = "spl_";
                        break;
                    default:
                        assert(false);
                        t1 = "";
                        break;
                }
                out << t1 << parent.getId() << " -> " << typ << child.getId() << '\n';
            };
            ProofNode * r1 = node->getAnt1();
            ProofNode * r2 = node->getAnt2();
            if (r1 != nullptr) {
                addEdgeToParent(*r1, *node);
                // Enqueue the parent
                q.push_back(r1);
            }
            if (r2 != nullptr) {
                addEdgeToParent(*r2, *node);
                // Enqueue the parent
                q.push_back(r2);
            }
            //Mark node as visited
            visited_dotty[node->getId()] = true;
        }
    }
    out << "}" << '\n';
}

void ProofGraph::printClause(ProofNode* n)
{
	assert(n);
	std::vector<Lit>& cl=n->getClause();
	zusmt::rerr() << n->getId();
	if(!n->isLeaf()) zusmt::rerr() << "(" << n->getAnt1()->getId() << "," << n->getAnt2()->getId() << ")";
	zusmt::rerr() << ": ";
	for(size_t k=0;k<cl.size();k++)
	{
		if(sign(cl[k])) zusmt::rerr() << "-";
		zusmt::rerr() << var(cl[k]) << " ";
	}
	zusmt::rerr() << '\n';
}

void ProofGraph::printClause(ProofNode* n, std::ostream & os)
{
	assert(n);
	std::vector<Lit> & cl = n->getClause();
	for (size_t k = 0 ; k < cl.size(); k++)
	{
		if(sign(cl[k])) { os << "-"; }
		os << var(cl[k]) << " ";
	}
}

void ProofGraph::printRuleApplicationStatus()
{
	zusmt::rerr() << "# Rules application status " << '\n';
	zusmt::rerr() << "# A1:           " << A1 << '\n';
	zusmt::rerr() << "# A1prime:      " << A1prime << '\n';
	zusmt::rerr() << "# A1B:          " << A1B << '\n';
	zusmt::rerr() << "# A2:           " << A2 << '\n';
	zusmt::rerr() << "# A2B:          " << A2B << '\n';
	zusmt::rerr() << "# A2U:          " << A2U << '\n';
	zusmt::rerr() << "# B1:           " << B1 << '\n';
	zusmt::rerr() << "# B2prime:      " << B2prime << '\n';
	zusmt::rerr() << "# B2:           " << B2 << '\n';
	zusmt::rerr() << "# B3:           " << B3 << '\n';
	zusmt::rerr() << "# duplications: " << duplications << '\n';
	zusmt::rerr() << "# swap_ties:    " << swap_ties << '\n';
}

}
