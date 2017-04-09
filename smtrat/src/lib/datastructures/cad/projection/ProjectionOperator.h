#pragma once

#include "../Common.h"

namespace smtrat {
namespace cad {

    struct ProjectionOperator {
        template<typename Callback>
        void operator()(ProjectionType pt, const UPoly& p, carl::Variable::Arg variable, Callback&& cb) const {
            switch (pt) {
				case ProjectionType::Brown: return Brown(p, variable, cb);
                case ProjectionType::McCallum: return McCallum(p, variable, cb);
                default:
                    SMTRAT_LOG_ERROR("smtrat.cad", "Selected a projection operator that is not implemented.");
                    return;
            }
        }
        template<typename Callback>
        void operator()(ProjectionType pt, const UPoly& p, const UPoly& q, carl::Variable::Arg variable, Callback&& i) const {
            switch (pt) {
				case ProjectionType::Brown: return Brown(p, q, variable, i);
                case ProjectionType::McCallum: return McCallum(p, q, variable, i);
                default:
                    SMTRAT_LOG_ERROR("smtrat.cad", "Selected a projection operator that is not implemented.");
                    return;
            }
        }

        /**
         * Tries to determine whether the given UPolynomial vanishes for some assignment.
         * Returns true if the UPolynomial is guaranteed not to vanish.
         * Returns false otherwise.
         */
        template<typename UPolyCoeff>
        bool doesNotVanish(const UPolyCoeff& p) const {
            if (p.isZero()) return false;
            if (p.isConstant()) return true;
            auto def = p.definiteness();
            if (def == carl::Definiteness::POSITIVE) return true;
            if (def == carl::Definiteness::NEGATIVE) return true;
            return false;
        }
		
		UPoly normalize(const UPoly& p) const {
			SMTRAT_LOG_DEBUG("smtrat.cad.projection", "Normalizing " << p << " to " << p.squareFreePart().pseudoPrimpart().normalized());
			return p.squareFreePart().pseudoPrimpart().normalized();
		}

		template<typename Callback>
		void Brown(const UPoly& p, const UPoly& q, carl::Variable::Arg variable, Callback& cb) const {
			auto res = normalize(p.resultant(q).switchVariable(variable));
			SMTRAT_LOG_DEBUG("smtrat.cad.projection", "resultant(" << p << ", " << q << ") = " << res);
			cb(res);
		}
		template<typename Callback>
		void Brown(const UPoly& p, carl::Variable::Arg variable, Callback& cb) const {
			// Insert discriminant
			auto dis = normalize(p.discriminant().switchVariable(variable));
			SMTRAT_LOG_DEBUG("smtrat.cad.projection", "discriminant(" << p << ") = " << dis);
			cb(dis);
			if (doesNotVanish(p.lcoeff())) {
				SMTRAT_LOG_DEBUG("smtrat.cad.projection", "lcoeff = " << p.lcoeff() << " does not vanish. No further UPolynomials needed.");
				return;
			}
			for (const auto& coeff: p.coefficients()) {
				if (doesNotVanish(coeff)) {
					auto lcoeff = normalize(p.lcoeff().toUnivariatePolynomial(variable));
					SMTRAT_LOG_DEBUG("smtrat.cad.projection", "coeff " << coeff << " does not vanish. We only need the lcoeff() = " << lcoeff);
					cb(lcoeff);
					return;
				}
			}
			SMTRAT_LOG_DEBUG("smtrat.cad.projection", "All coefficients might vanish, we need all of them.");
			for (const auto& coeff: p.coefficients()) {
				if (coeff.isConstant()) continue;
				SMTRAT_LOG_DEBUG("smtrat.cad.projection", "\t-> " << coeff);
				cb(normalize(coeff.toUnivariatePolynomial(variable)));
			}
		}
        template<typename Callback>
        void McCallum(const UPoly& p, const UPoly& q, carl::Variable::Arg variable, Callback& cb) const {
			SMTRAT_LOG_DEBUG("smtrat.cad.projection", "resultant(" << p << ", " << q << ")");
            cb(normalize(p.resultant(q).switchVariable(variable)));
        }
        template<typename Callback>
        void McCallum(const UPoly& p, carl::Variable::Arg variable, Callback& cb) const {
            // Insert discriminant
			SMTRAT_LOG_DEBUG("smtrat.cad.projection", "discriminant(" << p << ")");
            cb(p.discriminant().switchVariable(variable));
            for (const auto& coeff: p.coefficients()) {
				if (coeff.isConstant()) continue;
				SMTRAT_LOG_DEBUG("smtrat.cad.projection", "\t-> " << coeff);
                cb(normalize(coeff.toUnivariatePolynomial(variable)));
            }
        }
    };

}
}
