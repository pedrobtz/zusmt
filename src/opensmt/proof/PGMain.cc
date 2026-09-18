#include <r_compat.h>
/*
 *  Copyright (c) 2013, Simone Fulvio Rollini <simone.rollini@gmail.com>
 *
 *  SPDX-License-Identifier: MIT
 *
 */

#include "PG.h"

#include <common/SystemQueries.h>

namespace opensmt {

// Manipulates proofs
void ProofGraph::printProofGraph( )
{
	assert( printProofDotty() > 0);
	// Fill proof
	fillProofGraph();
	//Print original proof
	if( verbose() > 0 ) zusmt::rerr() << "# Outputting dotty proof" << '\n';
	std::ofstream dotty( "proof.dot", std::ofstream::out | std::ofstream::trunc);
	printProofAsDotty( dotty );
	emptyProofGraph();
}

void ProofGraph::transfProofForReduction( )
{
	// Initialize C-random number generator with fixed seed to ensure reproducibility
	zusmt::pseudo_srand(config.getRandomSeed());
	// Fill proof
	fillProofGraph();

	// Time left for transformation
	double solvingTime = cpuTime( );

//	size_t size=0;
	int numnodes=0;
	int numedges=0;
	int numleaves=0;
	int numvars=0;
	double avgdeg=0;
	int maxclasize=0;
	double avgclasize=0;
	int numunary=0;
//	double varclasize=0;

	if ( verbose() )
	{
		getGraphInfo( );

//		size = graph.size( );
		numnodes = num_nodes;
		numedges = num_edges;
		numleaves = num_leaves;
		numvars = proof_variables.size();
		avgdeg = (double)numedges / (double)numnodes;
		maxclasize = max_cla_size;
		avgclasize = av_cla_size;
		numunary = num_unary;
//		varclasize = var_cla_size;
	}

	double time=0;

	time = doReduction( solvingTime );

	if( proofCheck() )
	{
		checkProof( true );
		unsigned rem = cleanProofGraph( );
		if(verbose() > 0) zusmt::rerr() << "# Cleaned " << rem << " residual nodes"  << '\n';
		if(rem > 0) checkProof( true );
	}

	if ( verbose() > 0 )
	{
		getGraphInfo( );

		double perc_nodes=(((double)num_nodes-(double)numnodes)/(double)numnodes)*100;
		double perc_edges=(((double)num_edges-(double)numedges)/(double)numedges)*100;
		double perc_leaves=(((double)num_leaves-(double)numleaves)/(double)numleaves)*100;
		zusmt::rerr() << "#" << '\n';
		zusmt::rerr() << "# ------------------------------------" << '\n';
		zusmt::rerr() << "# PROOF GRAPH REDUCTION STATISTICS    " << '\n';
		zusmt::rerr() << "# ------------------------------------" << '\n';
		zusmt::rerr() << "# Structural properties" << '\n';
		zusmt::rerr() << "# ---------------------" << '\n';
		zusmt::rerr() << "# Actual num proof variables.: ";
		REprintf("%-10d %-10d\n", numvars, (int)proof_variables.size() );
		zusmt::rerr() << "# Nodes......................: ";
		REprintf("%-10d %-10d\n", numnodes, num_nodes );
		zusmt::rerr() << "# Nodes variation............: ";
		REprintf("%-9.2f %%\n", perc_nodes );
		zusmt::rerr() << "# Leaves.....................: ";
		REprintf("%-10d %-10d\n", numleaves, num_leaves );
		zusmt::rerr() << "# Leaves variation...........: ";
		REprintf("%-9.2f %%\n", perc_leaves );
		zusmt::rerr() << "# Edges......................: ";
		REprintf("%-10d %-10d\n", numedges, num_edges );
		zusmt::rerr() << "# Edges variation............: ";
		REprintf("%-9.2f %%\n", perc_edges );
		//zusmt::rerr() << "# Graph vector size..........: ";
		//REprintf("%-10ld %-10ld\n", size, graph.size( ) );
		zusmt::rerr() << "# Average degree.............: ";
		REprintf("%-10.2f %-10.2f\n", avgdeg, (double)num_edges / (double)num_nodes );
		zusmt::rerr() << "# Unary clauses..............: ";
		REprintf("%-10d %-10d\n", numunary, num_unary );
		zusmt::rerr() << "# Max clause size............: ";
		REprintf("%-10d %-10d\n", maxclasize, max_cla_size );
		zusmt::rerr() << "# Average clause size........: ";
		REprintf("%-10.2f %-10.2f\n", avgclasize, av_cla_size );
		//zusmt::rerr() << "# Variance clause size.......: ";
		//REprintf("%-10.2f %-10.2f\n", varclasize, var_cla_size );
		zusmt::rerr() << "# -------------------------" << '\n';
		zusmt::rerr() << "# Transformation statistics" << '\n';
		zusmt::rerr() << "# -------------------------" << '\n';
		zusmt::rerr() << "# Graph building time........: " << building_time << " s" << '\n';
		zusmt::rerr() << "# Transformation time........: " << time << " s" << '\n';
		//zusmt::rerr() << "# Duplications...............: " << num_dup << '\n';
		//zusmt::rerr() << "# Node additions due to A1...: " << num_node_add_A1 << '\n';
		zusmt::rerr() << "# ---------------------------" << '\n';
		zusmt::rerr() << "# Rules application statistics" << '\n';
		zusmt::rerr() << "# ---------------------------" << '\n';
		zusmt::rerr() << "# A1.........................: " << A1 << '\n';
		zusmt::rerr() << "# A1'........................: " << A1prime << '\n';
		zusmt::rerr() << "# A1 to B....................: " << A1B << '\n';
		zusmt::rerr() << "# A2.........................: " << A2 << '\n';
		zusmt::rerr() << "# A2 to B....................: " << A2B << '\n';
		zusmt::rerr() << "# A2 unary...................: " << A2U << '\n';
		zusmt::rerr() << "# B1.........................: " << B1 << '\n';
		zusmt::rerr() << "# B2'........................: " << B2prime << '\n';
		zusmt::rerr() << "# B2.........................: " << B2 << '\n';
		zusmt::rerr() << "# B3.........................: " << B3 << '\n';
		zusmt::rerr() << "# Duplications...............: " << duplications << '\n';
		zusmt::rerr() << "# Swap ties..................: " << swap_ties << '\n';
		zusmt::rerr() << "# ---------------------------" << '\n';
	}

	/*	if ( verbose() > 0 )
	{
		uint64_t mem_used = memUsed();
		reportff( "# Memory used after reducing the proof: %.3f MB\n",  mem_used == 0 ? 0 : mem_used / 1048576.0 );
	}*/

	if( printProofDotty() == 1 )
	{
		//Print reduced proof
		if( verbose() > 0 ) zusmt::rerr() << "# Outputting dotty proof reduced" << '\n';
        std::ofstream dottyred( "proof_reduced.dot" );
		printProofAsDotty( dottyred );
	}
	// TODO return reduced proof in SMTLIB2 format
	// Normalize antecedents order ( for interpolation )
	normalizeAntecedentOrder();
	// Empty proof
	emptyProofGraph();
}

void ProofGraph::transfProofForCNFInterpolants(std::function<icolor_t(Var)> getVarClass) {
    if (verbose() > 0) zusmt::rerr() << "; Proof transformation for interpolants (partially) in CNF" << '\n';

    fillProofGraph();
    proofTransformAndRestructure(-1, -1, true, [this, &getVarClass](RuleContext & ra1, RuleContext & ra2) {
        return this->handleRuleApplicationForCNFinterpolant(ra1, ra2, [this, &getVarClass](RuleContext & ra) {
            return this->allowSwapRuleForCNFinterpolant(ra, getVarClass);
        });
    });
    checkProof(true);
    normalizeAntecedentOrder();
    emptyProofGraph();
    printRuleApplicationStatus();
}

// Performs reduction
double ProofGraph::doReduction(double solving_time) {
    if (enabledTransfTraversals()) {
        if ((ratioReductionSolvingTime() > 0 && reductionTime() > 0) ||
            (ratioReductionSolvingTime() > 0 && numGraphTraversals() > 0) ||
            (reductionTime() > 0 && numGraphTraversals() > 0) ||
            (ratioReductionSolvingTime() == 0 && reductionTime() == 0 && numGraphTraversals() == 0)) {
                throw ApiException("Please set either ratio or time for reduction or number of proof traversals");
        }

    }
    if (reductionLoops() == 0) throw ApiException("Please set number of global reduction loops to at least 1");

    //Transformation time calculation
    double time_init = 0;
    double time_end = 0;
    double red_time = 0;
    //Number of inner transformation loops
    //-1 for exhaustiveness
    //int num_transf_loops=0; // MB: not used?
    //Number of outer transformation loops
    //useful for alternation with recycle pivots
    int num_global_reduction_loops = 0;
    // Time available for transformations
    // -1 for exhaustiveness
    double ratio;

    if (ratioReductionSolvingTime() > 0) {
        // Ratio transformation time/solving time
        ratio = ratioReductionSolvingTime();
        red_time = ratio * solving_time;
    } else if (reductionTime() > 0) {
        red_time = reductionTime();
    }

    //For each outer loop, recycle pivots algorithm is executed, followed by a certain
    //number of transformation loops, or by a single restructuring loop

    //Each global loop is given an equal fraction of available time
    num_global_reduction_loops = reductionLoops();
    if (verbose() > 0) {
        zusmt::rerr() << "# Compressing proof, " << num_global_reduction_loops << " global iteration(s) " << '\n';
        if (enabledPushDownUnits()) zusmt::rerr() << "# preceded by LowerUnits" << '\n';
        zusmt::rerr() << "# Each global iteration consists of: " << '\n';
        if (enabledStructuralHashing()) zusmt::rerr() << "# StructuralHashing" << '\n';
        if (enabledRecyclePivots()) zusmt::rerr() << "# RecyclePivotsWithIntersection" << '\n';
        if (enabledTransfTraversals()) {
            zusmt::rerr() << "# ReduceAndExpose ";
            if (ratioReductionSolvingTime() > 0 || reductionTime() > 0)
                zusmt::rerr() << "with overall timeout " << red_time << " sec(s) " << '\n';
            else if (numGraphTraversals() > 0)
                zusmt::rerr() << "with " << numGraphTraversals() << " graph traversal(s) " << '\n';
        }
        zusmt::rerr() << "#" << '\n';
    }
    double spent_time = 0, i_time = 0;

    time_init = cpuTime();
    if (enabledPushDownUnits()) recycleUnits();
    for (int k = 1; k <= num_global_reduction_loops; k++) {
        if (verbose() > 0) zusmt::rerr() << "# Global iteration " << k << '\n';
        i_time = cpuTime();
        if (switchToRPHashing()) {
            if (enabledRecyclePivots()) recyclePivotsIter();
            if (enabledStructuralHashing()) proofPostStructuralHashing();
        } else {
            if (enabledStructuralHashing()) proofPostStructuralHashing();
            if (enabledRecyclePivots()) recyclePivotsIter();
        }
        spent_time = cpuTime() - i_time;
        // Not really meaningful to do graph traversals in the last global loop
        if (enabledTransfTraversals() && k <= num_global_reduction_loops - 1) {
            auto handleRuleApplication = [this](RuleContext & ra1, RuleContext & ra2) {
                return this->handleRuleApplicationForReduction(ra1, ra2);
            };
            if (ratioReductionSolvingTime() > 0 || reductionTime() > 0) {
                // Available time = global loop timeout - time used for recycle pivots
                // Already out of time for transformation rules
                if (red_time - spent_time <= 0) { continue; }
                proofTransformAndRestructure(red_time - spent_time, -1, true, handleRuleApplication);
            }
            if (numGraphTraversals() > 0) {
                proofTransformAndRestructure(-1, numGraphTraversals(), true, handleRuleApplication);
            }
        }
    }
    time_end = cpuTime();
    return time_end - time_init;
}

}
