
/*
 * Copyright (C) 2000-2001 QuantLib Group
 *
 * This file is part of QuantLib.
 * QuantLib is a C++ open source library for financial quantitative
 * analysts and developers --- http://quantlib.sourceforge.net/
 *
 * QuantLib is free software and you are allowed to use, copy, modify, merge,
 * publish, distribute, and/or sell copies of it under the conditions stated
 * in the QuantLib License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the license for more details.
 *
 * You should have received a copy of the license along with this file;
 * if not, contact ferdinando@ametrano.net
 * The license is also available at http://quantlib.sourceforge.net/LICENSE.TXT
 *
 * The members of the QuantLib Group are listed in the Authors.txt file, also
 * available at http://quantlib.sourceforge.net/Authors.txt
*/

/*! \file ridder.cpp
    \brief Ridder 1-D solver

    $Source$
    $Log$
    Revision 1.22  2001/05/09 11:06:19  nando
    A few comments modified/removed

    Revision 1.21  2001/04/09 14:13:34  nando
    all the *.hpp moved below the Include/ql level

    Revision 1.20  2001/04/06 18:46:22  nando
    changed Authors, Contributors, Licence and copyright header

    Revision 1.19  2001/04/04 13:32:54  enri
    tons of typos fixed

    Revision 1.18  2001/04/04 12:13:24  nando
    Headers policy part 2:
    The Include directory is added to the compiler's include search path.
    Then both your code and user code specifies the sub-directory in
    #include directives, as in
    #include <Solvers1d/newton.hpp>

    Revision 1.17  2001/04/04 11:07:24  nando
    Headers policy part 1:
    Headers should have a .hpp (lowercase) filename extension
    All *.h renamed to *.hpp

*/

/* The implementation of the algorithm was inspired by
 * "Numerical Recipes in C", 2nd edition, Press, Teukolsky, Vetterling, Flannery
 * Chapter 9
 */


#include "ql/Solvers1D/ridder.hpp"

namespace QuantLib {

    namespace Solvers1D {

        #define SIGN(a,b) ((b) >= 0.0 ? QL_FABS(a) : -QL_FABS(a))

        double Ridder::solve_(const ObjectiveFunction& f, double xAcc) const {
            double fxMid, froot, s, xMid, nextRoot;

            // test on black scholes implied vol show that Ridder solver
            // algorythm actually provides an accuracy 100 times below promised
            double xAccuracy = xAcc/100.0;

            // Any highly unlikely value, to simplify logic below
            root_=QL_MIN_DOUBLE;

            while (evaluationNumber_<=maxEvaluations_) {
                xMid=0.5*(xMin_+xMax_);
                // First of two function evaluations per iteraton
                fxMid=f(xMid);
                evaluationNumber_++;
                s=QL_SQRT(fxMid*fxMid-fxMin_*fxMax_);
                if (s == 0.0)
                    return root_;
                // Updating formula
                nextRoot = xMid + (xMid - xMin_) *
                            ((fxMin_ >= fxMax_ ? 1.0 : -1.0) * fxMid / s);
                if (QL_FABS(nextRoot-root_) <= xAccuracy)
                    return root_;

                root_=nextRoot;
                // Second of two function evaluations per iteration
                froot=f(root_);
                evaluationNumber_++;
                if (froot == 0.0)
                    return root_;

                // Bookkeeping to keep the root bracketed on next iteration
                if (SIGN(fxMid,froot) != fxMid) {
                    xMin_=xMid;
                    fxMin_=fxMid;
                    xMax_=root_;
                    fxMax_=froot;
                } else if (SIGN(fxMin_,froot) != fxMin_) {
                    xMax_=root_;
                    fxMax_=froot;
                } else if (SIGN(fxMax_,froot) != fxMax_) {
                    xMin_=root_;
                    fxMin_=froot;
                } else
                    throw Error("Ridder: never get here.");

                if (QL_FABS(xMax_-xMin_) <= xAccuracy) return root_;
            }
            throw Error("Ridder: maximum number of function evaluations (" +
             IntegerFormatter::toString(maxEvaluations_) + ") exceeded");

            QL_DUMMY_RETURN(0.0);
        }

    }

}
