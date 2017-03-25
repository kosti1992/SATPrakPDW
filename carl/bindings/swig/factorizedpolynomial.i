



%module FactorizedPolynomialRationalT
%{
#include <carl/core/MultivariatePolynomial.h>
#include <carl/core/FactorizedPolynomial.h>
#include "gmp.h"
#include "gmpxx.h"




typedef mpq_class Rational;


typedef carl::Term<Rational> Term;
typedef carl::MultivariatePolynomial<Rational> Polynomial;
typedef carl::FactorizedPolynomial<Polynomial> FactorizedPolynomial;
typedef carl::PolynomialFactorizationPair<Polynomial> FactorizationPair;

typedef unsigned int uint;
typedef std::pair<carl::Variable,uint> VarIntPair;


typedef FactorizedPolynomial::CACHE FactorizationCache;


%}

%include "std_string.i"
%include "std_shared_ptr.i"
%shared_ptr(carl::Cache<FactorizationPair>);

%import "variable.i"
%import "polynomial.i"

typedef mpq_class Rational;
typedef carl::Term<Rational> Term;
typedef carl::MultivariatePolynomial<Rational> Polynomial;
typedef carl::FactorizedPolynomial<Polynomial> FactorizedPolynomial;
typedef carl::PolynomialFactorizationPair<Polynomial> FactorizationPair;

typedef FactorizedPolynomial::CACHE FactorizationCache;



namespace carl {

%rename(FactorizationCache) Cache<FactorizationPair>;
class Cache<FactorizationPair> {
public:
Cache( size_t _maxCacheSize = 10000, double _cacheReductionAmount = 0.2, double _decay = 0.98 );

};





template<typename P>
class FactorizedPolynomial {
public:
typedef typename P::CoeffType CoeffType;
typedef carl::Cache<carl::PolynomialFactorizationPair<P>> CACHE;


explicit FactorizedPolynomial( const CoeffType& );

explicit FactorizedPolynomial( const P& _polynomial, const std::shared_ptr<carl::FactorizedPolynomial::CACHE>&, bool = false );
CoeffType constantPart() const;
template<typename SubstitutionType = CoeffType>
SubstitutionType evaluate(const std::map<carl::Variable, SubstitutionType>& substitutions) const;
        


std::string toString( bool _infix = true, bool _friendlyVarNames = true ) const;

%extend {
std::vector<carl::Variable> gatherVariables() const {
   std::set<carl::Variable> asSet = $self->gatherVariables();
   return std::vector<carl::Variable>(asSet.begin(),asSet.end());
}

}

%extend {

carl::FactorizedPolynomial<P> add(const carl::FactorizedPolynomial<P>& rhs) {
	return *$self+rhs;
}

carl::FactorizedPolynomial<P> sub(const carl::FactorizedPolynomial<P>& rhs) {
	return *$self-rhs;
}

carl::FactorizedPolynomial<P> mul(const carl::FactorizedPolynomial<P>& rhs) {
	return *$self*rhs;
}


bool equals(const carl::FactorizedPolynomial<P>& rhs) {
	return *$self==rhs;
}

bool notEquals(const carl::FactorizedPolynomial<P>& rhs) {
	return *$self!=rhs;
}




}

};

%template(evaluate) carl::FactorizedPolynomial::evaluate<Rational>;

}

%template(FactorizedPolynomialRational) carl::FactorizedPolynomial<Polynomial>;

