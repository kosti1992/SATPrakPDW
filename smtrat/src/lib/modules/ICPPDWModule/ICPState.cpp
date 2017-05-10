#include "ICPState.h"
#include "ICPContractionCandidate.h"

namespace smtrat
{
    ICPState::ICPState() {
    }

    ICPState::~ICPState() {
    }

    vector<ICPContractionCandidate*>& ICPState::getContractionCandidates() {
        return mContractionCandidates;
    }

    void ICPState::addContractionCandidate(ICPContractionCandidate* contractionCandidate) {
        mContractionCandidates.push_back(contractionCandidate);
    }

    carl::Variable ICPState::getSplitDimension() {
        return mSplitDimension;
    }

    void ICPState::setSplitDimension(carl::Variable splitDimension) {
        mSplitDimension = splitDimension;
    }

    std::set<ConstraintT>& ICPState::getConflictingConstraints() {
        return mConflictingConstraints;
    }

    void ICPState::addConflictingConstraint(const ConstraintT& constraint) {
        mConflictingConstraints.insert(constraint);
    }
}
