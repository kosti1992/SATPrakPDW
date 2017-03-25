

%module RationalT 
%{
#include <carl/core/Monomial.h>
#include <carl/core/Term.h>
#include <carl/core/MultivariatePolynomial.h>
#include <carl/core/RationalFunction.h>
#include <carl/numbers/numbers.h>
#include "gmp.h"
#include "gmpxx.h"


typedef mpq_class Rational;

typedef carl::Term<Rational> Term;

typedef carl::MultivariatePolynomial<Rational> Polynomial;
typedef carl::RationalFunction<Polynomial> RationalFunction; 

%}




%include "std_string.i"
%import "monomial.i"
%import "variable.i"
%import "term.i"
%import "polynomial.i"
%import "rationalfunction.i"


namespace carl {
typedef long sint;
}


%rename(Rational) mpq_class;

class mpq_class {
//NOTE: maybe change this to getDouble or whatever..
%rename(toDouble) get_d;
public:
mpq_class(signed int i) { mpq_init(mp); mpq_set_si(mp, i, 1); }
mpq_class(double d) { mpq_init(mp); mpq_set_d(mp, d); }
  explicit mpq_class(const std::string &s, int base = 0)
  {
    mpq_init(mp);
    if (mpq_set_str (mp, s.c_str(), base) != 0)
      {
        mpq_clear (mp);
        throw std::invalid_argument ("mpq_set_str");
      }
  }

 double get_d() const;

%extend{
 std::string toString() {
   return carl::toString(*$self,true);	
 } 



int nominator() {
 return carl::toInt<carl::sint>(carl::getNum(*$self));
}

int numerator() {
 return carl::toInt<carl::sint>(carl::getNum(*$self));
}

int denominator() {
 return carl::toInt<carl::sint>(carl::getDenom(*$self));
}


carl::sint toInt() {
    double d = carl::toDouble(*$self);
    return static_cast<carl::sint>(d);
}



	Polynomial add(const Polynomial & rhs) {
		return *$self+rhs;
	}

	Polynomial  add(const Term& rhs) {
		return *$self+rhs;
	} 

	Polynomial add(const carl::Monomial::Arg& rhs) {
		return *$self+rhs;
	}


	Polynomial add(carl::Variable::Arg rhs) {
		return *$self+rhs;
	}


	Rational add(const Rational& rhs) {
		return *$self+rhs;
	} 



	Polynomial  sub(const Polynomial & rhs) {
		return *$self-rhs;
	}

	Polynomial  sub(const Term& rhs) {
		return *$self-rhs;
	}

	Polynomial sub(const carl::Monomial::Arg& rhs) {
		return *$self-rhs;
	}


	Polynomial  sub(carl::Variable::Arg rhs) {
		return *$self-rhs;
	}


	Rational sub(const Rational& rhs) {
		return *$self-rhs;
	} 


	Polynomial mul(const Polynomial & rhs) {
		return *$self*rhs;
	}

	Term mul(const Term& rhs) {
		return *$self*rhs;
	}

	Term mul(const carl::Monomial::Arg& rhs) {
		return *$self*rhs;
	}


	Term mul(carl::Variable::Arg rhs) {
		return *$self*rhs;
	} 


	Rational mul(const Rational& rhs) {
		return *$self*rhs;
	} 


	RationalFunction div(const RationalFunction& rhs) {
		return RationalFunction(*$self) / rhs;
	}

	RationalFunction div(const Polynomial& rhs) {
		return RationalFunction(*$self) / rhs;
	}

	RationalFunction div(const Term& rhs) {
		return RationalFunction(*$self) / rhs;
	}

	RationalFunction div(const carl::Monomial::Arg& rhs) {
		return RationalFunction(*$self) / rhs;
	}


	RationalFunction div(carl::Variable::Arg rhs) {
		return RationalFunction(*$self) / rhs;
	} 


	Rational div(const Rational& rhs) {
		if (carl::isZero(rhs)) throw std::runtime_error("Div by zero");
		return *$self / rhs;
	}  

	Rational pow(std::size_t exp) {
		return pow($self->get_d(),exp);
	}	

	Rational neg() {
		return -(*$self);
	}

	Rational abs() {
		return abs(*$self);
	}

	bool equals(const Rational& rhs) {
		return *$self==rhs;
	}

	bool notEquals(const Rational& rhs) {
		return *$self!=rhs;
	}


	bool lessEquals(const Rational&  rhs) {
		return *$self<=rhs;
	}

	bool greaterEquals(const Rational&  rhs) {
		return *$self>=rhs;
	}


	bool less(const Rational& rhs) {
		return *$self<rhs;
	}

	bool greater(const Rational& rhs) {
		return *$self>rhs;
	} 




}

}; 





typedef mpq_class Rational;

/*
typedef carl::Term<Rational> Term;

typedef carl::MultivariatePolynomial<Rational> Polynomial;
typedef carl::RationalFunction<Polynomial> RationalFunction;  */
