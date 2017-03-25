#pragma once


#include "../../util/variant_util.h"
#include "../bitvector/BVValue.h"
#include "mvroot/MultivariateRoot.h"
#include "ran/RealAlgebraicNumber.h"
#include "sqrtex/SqrtEx.h"
#include "uninterpreted/SortValue.h"
#include "uninterpreted/UFModel.h"

#include <boost/variant.hpp>

#include <memory>

namespace carl
{
	template<typename Rational, typename Poly>
	class ModelValue;
	template<typename Rational, typename Poly>
	class ModelSubstitution;
	template<typename Rational, typename Poly>
	class ModelMVRootSubstitution;
	template<typename Rational, typename Poly>
	using ModelSubstitutionPtr = std::shared_ptr<ModelSubstitution<Rational,Poly>>;
	template<typename Rational, typename Poly, typename Substitution, typename... Args>
	inline ModelValue<Rational,Poly> createSubstitution(Args&&... args);
	template<typename Rational, typename Poly>
	inline ModelValue<Rational,Poly> createSubstitution(const MultivariateRoot<Poly>& mr);

	/**
	 * This class represents infinity or minus infinity, depending on its flag positive.
	 * The default is minus infinity.
	 */
	struct InfinityValue {
		bool positive = false;
		explicit InfinityValue() {}
		explicit InfinityValue(bool positive): positive(positive) {}
	};
    
	inline std::string toString(const InfinityValue& iv, bool _infix) {
        if( _infix )
        {
            std::string result = iv.positive ? "+" : "-";
            result += "infinity";
            return result;
        }
        if( iv.positive )
            return "infinity";
        return "(- infinity)";
	}
    
	inline std::ostream& operator<<(std::ostream& os, const InfinityValue& iv) {
		return os << (iv.positive ? "+" : "-") << "infinity";
	}
	
    /**
     * This class represents some value that is assigned to some variable.
     * It is implemented as subclass of a boost::variant.
     * Possible value types are bool, vs::SqrtEx and carl::RealAlgebraicNumberPtr.
     */
	template<typename Rational, typename Poly>
    class ModelValue : public boost::variant<bool, Rational, SqrtEx<Poly>, RealAlgebraicNumber<Rational>, BVValue, SortValue, UFModel, InfinityValue, ModelSubstitutionPtr<Rational,Poly>>
    {
        /**
         * Base type we are deriving from.
         */
        using Super = boost::variant<bool, Rational, SqrtEx<Poly>, RealAlgebraicNumber<Rational>, BVValue, SortValue, UFModel, InfinityValue, ModelSubstitutionPtr<Rational,Poly>>;
        
    public:
        /**
         * Default constructor.
         */
        ModelValue(): Super()
        {}

        /**
         * Initializes the Assignment from some valid type of the underlying variant.
         */
        template<typename T>
        ModelValue(const T& _t): Super(_t)
        {}
		
		template<typename ...Args>
		ModelValue(const boost::variant<Args...>& variant): Super(variant_extend<Super>::extend(variant_extend<ModelValue<Rational,Poly>>::extend(variant))) {}
		
		ModelValue(const MultivariateRoot<Poly>& mr): Super(createSubstitution<Rational,Poly>(mr).asSubstitution()) {}
		
        /**
         * Assign some value to the underlying variant.
         * @param t Some value.
         * @return *this.
         */
        template<typename T>
        ModelValue& operator=(const T& _t) {
            Super::operator=(_t);
            return *this;
        }
		template<typename ...Args>
        ModelValue& operator=(const boost::variant<Args...>& variant) {
            Super::operator=(variant_extend<Super>::extend(variant_extend<ModelValue<Rational,Poly>>::extend(variant)));
            return *this;
        }

        /**
         * Check if two Assignments are equal.
         * Two Assignments are considered equal, if both are either bool or not bool and their value is the same.
         * 
         * If both Assignments are not bools, the check may return false although they represent the same value.
         * If both are numbers in different representations, this comparison is only done as a "best effort".
         * 
         * @param _ass Another Assignment.
         * @return *this == a.
         */
        bool operator==( const ModelValue& _mval ) const
        {
            if( isBool() && _mval.isBool() )
            {
                return asBool() == _mval.asBool();
            }
            else if( isRational() && _mval.isRational() )
            {
                return asRational() == _mval.asRational();
            }
            else if( isSqrtEx() && _mval.isSqrtEx() )
            {
                return asSqrtEx() == _mval.asSqrtEx();
            }
            else if( isRAN() & _mval.isRAN() )
            {
                return asRAN() == _mval.asRAN();
            }
			else if( isBVValue() && _mval.isBVValue() )
            {
                return asBVValue() == _mval.asBVValue();
            }
            else if( isSortValue() & _mval.isSortValue() )
            {
                return asSortValue() == _mval.asSortValue();
            }
            else if( isUFModel() & _mval.isUFModel() )
            {
                return asUFModel() == _mval.asUFModel();
            }
            return false;
        }

        /**
         * @return true, if the stored value is a bool.
         */
        bool isBool() const {
			return variant_is_type<bool>::check(*this);
        }
        
        /**
         * @return true, if the stored value is a rational.
         */
        bool isRational() const {
			return variant_is_type<Rational>::check(*this);
        }
        
        /**
         * @return true, if the stored value is a square root expression.
         */
        bool isSqrtEx() const {
			return variant_is_type<SqrtEx<Poly>>::check(*this);
        }
        
        /**
         * @return true, if the stored value is a real algebraic number.
         */
        bool isRAN() const {
			return variant_is_type<RealAlgebraicNumber<Rational>>::check(*this);
        }
        
        /**
         * @return true, if the stored value is a bitvector literal.
         */
        bool isBVValue() const {
			return variant_is_type<BVValue>::check(*this);
        }

        /**
         * @return true, if the stored value is a sort value.
         */
        bool isSortValue() const {
			return variant_is_type<SortValue>::check(*this);
        }
        
        /**
         * @return true, if the stored value is a uninterpreted function model.
         */
        bool isUFModel() const {
			return variant_is_type<UFModel>::check(*this);
        }
		
		/**
         * @return true, if the stored value is +infinity.
         */
        bool isPlusInfinity() const {
			return variant_is_type<InfinityValue>::check(*this) && boost::get<InfinityValue>(*this).positive;
        }
		/**
         * @return true, if the stored value is -infinity.
         */
        bool isMinusInfinity() const {
			return variant_is_type<InfinityValue>::check(*this) && !boost::get<InfinityValue>(*this).positive;
        }
		
		bool isSubstitution() const {
			return variant_is_type<ModelSubstitutionPtr<Rational,Poly>>::check(*this);
		}

        /**
         * @return The stored value as a bool.
         */
        bool asBool() const
        {
            assert( isBool() );
            return boost::get<bool>(*this);
        }
        
        /**
         * @return The stored value as a rational.
         */
        const Rational& asRational() const
        {
            assert( isRational() );
            return boost::get<Rational>(*this);
        }
        
        /**
         * @return The stored value as a square root expression.
         */
        const SqrtEx<Poly>& asSqrtEx() const
        {
            assert( isSqrtEx() );
            return boost::get<SqrtEx<Poly>>(*this);
        }
        
        /**
         * @return The stored value as a real algebraic number.
         */
        RealAlgebraicNumber<Rational> asRAN() const
        {
            assert( isRAN() );
            return boost::get<RealAlgebraicNumber<Rational>>(*this);
        }
        
        /**
         * @return The stored value as a real algebraic number.
         */
        const carl::BVValue& asBVValue() const
        {
            assert( isBVValue() );
            return boost::get<carl::BVValue>(*this);
        }

        /**
         * @return The stored value as a sort value.
         */
        const SortValue& asSortValue() const
        {
            assert( isSortValue() );
            return boost::get<SortValue>(*this);
        }
        
        /**
         * @return The stored value as a uninterpreted function model.
         */
        const UFModel& asUFModel() const
        {
            assert( isUFModel() );
            return boost::get<UFModel>(*this);
        }
		/**
         * @return The stored value as a infinity value.
         */
        const InfinityValue& asInfinity() const
        {
            assert( isPlusInfinity() || isMinusInfinity() );
            return boost::get<InfinityValue>(*this);
        }
		
		const ModelSubstitutionPtr<Rational,Poly>& asSubstitution() const {
			assert(isSubstitution());
			return boost::get<ModelSubstitutionPtr<Rational,Poly>>(*this);
		}
		ModelSubstitutionPtr<Rational,Poly>& asSubstitution() {
			assert(isSubstitution());
			return boost::get<ModelSubstitutionPtr<Rational,Poly>>(*this);
		}
        
    };
}
