#include "ICPState.h"
#include "ICPContractionCandidate.h"

namespace smtrat
{
    ICPState::ICPState() {
    }

    ICPState::ICPState(ICPContractionCandidate* contractionCandidate) :
        mContractionCandidate(contractionCandidate)
    {
    }

    ICPState::~ICPState() {
    }

    std::map<carl::Variable*, RationalInterval>* ICPState::getBox() {
        return &mSearchBox;
    }

    ICPContractionCandidate* ICPState::getContractionCandidate() {
        return mContractionCandidate;
    }

    void ICPState::setContractionCandidate(ICPContractionCandidate* contractionCandidate) {
        mContractionCandidate = contractionCandidate;
    }

    carl::Variable* ICPState::getSplitDimension() {
        return mSplitDimension;
    }

    void ICPState::setSplitDimension(carl::Variable* splitDimension) {
        mSplitDimension = splitDimension;
    }

    vector<ConstraintT*>* ICPState::getConflictingConstraints() {
        return &mConflictingConstraints;
    }

    void ICPState::addConflictingConstraint(ConstraintT* constraint) {
        mConflictingConstraints.push_back(constraint);
    }
}