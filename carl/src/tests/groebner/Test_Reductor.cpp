#include "gtest/gtest.h"
#include "carl/groebner/Reductor.h"
#include "carl/util/platform.h"

#include "../Common.h"

using namespace carl;

TEST(Reductor, Constructor)
{

}

TEST(Reductor, Reduction)
{
    Variable x = freshRealVariable("x");
	Variable y = freshRealVariable("y");
	Variable z = freshRealVariable("z");
    Ideal<MultivariatePolynomial<Rational>> ideal;
    MultivariatePolynomial<Rational> p1;
    p1 += x*x;
    p1 += z;
    MultivariatePolynomial<Rational> p2;
    p2 += y*y;
    ideal.addGenerator(p1);
    ideal.addGenerator(p2);
    MultivariatePolynomial<Rational> f;
    f += y;

    Reductor<MultivariatePolynomial<Rational>, MultivariatePolynomial<Rational>> reductor(ideal, f);
    MultivariatePolynomial<Rational> fres = reductor.fullReduce();
    EXPECT_EQ(f,fres);

    MultivariatePolynomial<Rational> f2;
    f2 += y*y;

    Reductor<MultivariatePolynomial<Rational>, MultivariatePolynomial<Rational>> reductor2(ideal, f2);
    fres = reductor2.fullReduce();
    EXPECT_EQ(MultivariatePolynomial<Rational>(),fres);

    MultivariatePolynomial<Rational> f3;
    f3 += x*z;
    Reductor<MultivariatePolynomial<Rational>, MultivariatePolynomial<Rational>> reductor3(ideal, f3);
    fres = reductor3.fullReduce();
    EXPECT_EQ(f3, fres);

    MultivariatePolynomial<Rational> f4;
    f4 += x*x;
    Reductor<MultivariatePolynomial<Rational>, MultivariatePolynomial<Rational>> reductor4(ideal, f4);
    fres = reductor4.fullReduce();
    EXPECT_EQ((Rational)-1 * z, fres);
}
