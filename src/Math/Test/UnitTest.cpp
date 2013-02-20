/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include <TestSuite/Tester.h>

#include "Math/Unit.h"

namespace Magnum { namespace Math { namespace Test {

class UnitTest: public Corrade::TestSuite::Tester {
    public:
        explicit UnitTest();

        void construct();
        void constructDefault();
        void constructConversion();
        void compare();

        void negated();
        void addSubtract();
        void multiplyDivide();
};

UnitTest::UnitTest() {
    addTests(&UnitTest::construct,
             &UnitTest::constructDefault,
             &UnitTest::constructConversion,
             &UnitTest::compare,

             &UnitTest::negated,
             &UnitTest::addSubtract,
             &UnitTest::multiplyDivide);
}

template<class> struct Sec_;
typedef Unit<Sec_, float> Sec;
typedef Unit<Sec_, double> Secd;

inline Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, Sec value) {
    return debug << float(value);
}

void UnitTest::construct() {
    constexpr Sec a(25.0f);
    CORRADE_COMPARE(float(a), 25.0f);
}

void UnitTest::constructDefault() {
    constexpr Sec b;
    CORRADE_COMPARE(b, Sec(0.0f));
}

void UnitTest::constructConversion() {
    constexpr Secd a(25.0);
    constexpr Sec b(a);
    CORRADE_COMPARE(b, Sec(25.0f));
}

void UnitTest::compare() {
    CORRADE_VERIFY(Sec(25.0f + MathTypeTraits<float>::epsilon()/2) == Sec(25.0f));
    CORRADE_VERIFY(Sec(25.0f + MathTypeTraits<float>::epsilon()*2) != Sec(25.0f));

    constexpr bool c = Sec(3.0f) < Sec(3.0f);
    constexpr bool d = Sec(3.0f) <= Sec(3.0f);
    constexpr bool e = Sec(3.0f) >= Sec(3.0f);
    constexpr bool f = Sec(3.0f) > Sec(3.0f);
    CORRADE_VERIFY(!c);
    CORRADE_VERIFY(d);
    CORRADE_VERIFY(e);
    CORRADE_VERIFY(!f);

    constexpr bool h = Sec(2.0f) < Sec(3.0f);
    constexpr bool i = Sec(2.0f) <= Sec(3.0f);
    constexpr bool j = Sec(3.0f) >= Sec(2.0f);
    constexpr bool k = Sec(3.0f) > Sec(2.0f);
    CORRADE_VERIFY(h);
    CORRADE_VERIFY(i);
    CORRADE_VERIFY(j);
    CORRADE_VERIFY(k);

    constexpr bool l = Sec(3.0f) < Sec(2.0f);
    constexpr bool m = Sec(3.0f) <= Sec(2.0f);
    constexpr bool n = Sec(2.0f) >= Sec(3.0f);
    constexpr bool o = Sec(2.0f) > Sec(3.0f);
    CORRADE_VERIFY(!l);
    CORRADE_VERIFY(!m);
    CORRADE_VERIFY(!n);
    CORRADE_VERIFY(!o);
}

void UnitTest::negated() {
    constexpr Sec a(25.0f);
    constexpr Sec b(-a);
    CORRADE_COMPARE(b, Sec(-25.0f));
}

void UnitTest::addSubtract() {
    constexpr Sec a(3.0f);
    constexpr Sec b(-4.0f);
    constexpr Sec c(-1.0f);

    constexpr Sec d = a + b;
    constexpr Sec e = c - a;
    CORRADE_COMPARE(d, c);
    CORRADE_COMPARE(e, b);
}

void UnitTest::multiplyDivide() {
    constexpr Sec a(3.0f);
    constexpr Sec b(-4.5f);
    constexpr Sec c(5.0f);

    constexpr Sec d = a*-1.5f;
    constexpr Sec e = -1.5f*a;
    constexpr Sec f = b/-1.5f;
    CORRADE_COMPARE(d, b);
    CORRADE_COMPARE(e, b);
    CORRADE_COMPARE(f, a);

    constexpr float g = b/a;
    CORRADE_COMPARE(g, -1.5f);
}

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::UnitTest)