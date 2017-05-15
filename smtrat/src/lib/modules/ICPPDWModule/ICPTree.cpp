#include "ICPTree.h"

namespace smtrat
{

    ICPTree::ICPTree() :
        mCurrentState(),
        mParentTree(nullptr),
        mChildTrees(),
        mSplitDimension()
    {
    }

    ICPTree::ICPTree(ICPTree* parent, const vb::VariableBounds<ConstraintT>& parentBounds) :
        mCurrentState(parentBounds),
        mParentTree(parent),
        mChildTrees(),
        mSplitDimension()
    {
    }

    bool ICPTree::contract(vector<ICPContractionCandidate>& contractionCandidates) {
        while(true) {
            std::cout << "\nVariable bounds:" << std::endl;
            for (const auto& mapEntry : mCurrentState.getBounds().getIntervalMap()){
                std::cout << mapEntry.first << " in " << mapEntry.second << std::endl;
            }

            // first we need to make sure the bounds are still satisfiable
            // i.e. no variable has an empty interval
            if (mCurrentState.getBounds().isConflicting()) {
                // if the bounds do contain a conflict, this ICP node is unsatisfiable
                // so we retrieve the set of conflicting constraints and add them to our state
                // TODO: This method only returns the last constraint that was used
                //       so implement a method which actually determines the unsat core
                mCurrentState.setConflictingConstraints(mCurrentState.getBounds().getConflict());

                std::cout << "Bounds are conflicting!" << std::endl;
                std::cout << "Reasons: " << std::endl;
                for (const ConstraintT& c : mCurrentState.getConflictingConstraints()) {
                    std::cout << c << ", ";
                }
                std::cout << std::endl;

                // TODO: propagate UNSAT if both child trees are unsat
                // or maybe do this in a seperate method

                // we will terminate, but we did not split the search space
                return false;
            }
            // if we met some other termination condition (e.g. target diameter reached
            else if (mCurrentState.isTerminationConditionReached()) {
                std::cout << "Termination condition reached." << std::endl;

                // we will terminate, but we did not split the search space
                return false;
            }
            // we can contract
            else {

                // We have to pick the best contraction candidate that we want to apply
                ICPContractionCandidate& bestCC = mCurrentState.getBestContractionCandidate(contractionCandidates);
                OneOrTwo<IntervalT> bounds = bestCC.getContractedInterval(mCurrentState.getBounds());

                if(bounds.second){
                    // TODO: Split!
                    //std::cout << cc << " results in bound: " << bounds.first << ":" << "Second Interval" << std::endl;
                    std::cout << "Contract with " << bestCC << ", results in bounds: " << bounds.first << ":" << *(bounds.second) << std::endl;
                    mCurrentState.applyContraction(&bestCC, bounds.first);
                } else {
                    // no split, we can simply apply the contraction to the current state
                    std::cout << "Contract with " << bestCC << ", results in bounds: " << bounds.first << std::endl;
                    mCurrentState.applyContraction(&bestCC, bounds.first);
                }

                //std::cout << "Contract with " << bestCC << ", results in bounds: " << bounds << std::endl;
                /*  TODO: THIS IS NOT WORKING. THIS RESULTS IN DOUBLE FREES. I BET IT'S VARIABLE BOUNDS FAULT!!
                // if the contraction results in two intervals, we need to split the search space
                // TODO: if (split occurred) {
                if (!bounds.second.isEmpty()) {

                    std::cout << "Split!" << std::endl;
                    // we store the dimension at which we split
                    mSplitDimension = bestCC.getVariable();

                    // we create one ICP child tree
                    mChildTrees.push_back(ICPTree(this, mCurrentState.getBounds()));
                    // on which we apply the first interval
                    mCurrentState.applyContraction(&cc, bounds.first);
                    std::cout << "Added left tree." << std::endl;

                    // and another ICP child tree
                    mChildTrees.push_back(ICPTree(this, mCurrentState.getBounds()));
                    // on which we apply the second interval
                    mCurrentState.applyContraction(&cc, bounds.second);
                    std::cout << "Added right tree." << std::endl;

                    // since a split occurred, we cannot contract the current ICP node any further
                    return true;
                }
                else {
                    // no split occurred, so we apply the only contracted interval
                    mCurrentState.applyContraction(&cc, bounds.first);
                }
                */
            }
        }
    }

    ICPState& ICPTree::getCurrentState() {
        return mCurrentState;
    }

    void ICPTree::setCurrentState(const ICPState& state) {
        mCurrentState = state;
    }

    ICPTree* ICPTree::getParentTree() {
        return mParentTree;
    }

    carl::Variable ICPTree::getSplitDimension() {
        return mSplitDimension;
    }

    void ICPTree::setSplitDimension(carl::Variable splitDimension) {
        mSplitDimension = splitDimension;
    }

    vector<ICPTree>& ICPTree::getChildTrees() {
        return mChildTrees;
    }

    bool ICPTree::isLeaf() {
        return mChildTrees.empty();
    }
}
