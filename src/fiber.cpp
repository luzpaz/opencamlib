/*  $Id$
 * 
 *  Copyright 2010 Anders Wallin (anders.e.e.wallin "at" gmail.com)
 *  
 *  This file is part of OpenCAMlib.
 *
 *  OpenCAMlib is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  OpenCAMlib is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with OpenCAMlib.  If not, see <http://www.gnu.org/licenses/>.
*/
//#include <iostream>
//#include <stdio.h>
//#include <sstream>
//#include <math.h>
//#include <boost/numeric/interval.hpp>
#include <boost/foreach.hpp>
#include <boost/python.hpp>

#include "fiber.h"
//namespace bn = boost::numeric;

namespace ocl
{

Interval::Interval()
{
    lower = 0.0;
    upper = 0.0;
    lower_cc = CCPoint();
    upper_cc = CCPoint();
}

Interval::Interval(double l, double u)
{
    assert( l <= u );
    lower = l;
    upper = u;
}

Interval::~Interval()
{
    return;
}

void Interval::updateUpper(double t, CCPoint& p) {
    if (upper_cc.type == NONE) {
        //std::cout << " I: updateUpper() NONE up=lo=" << t << "\n";
        upper = t;
        lower = t;
        upper_cc = p;
        lower_cc = p;
    }
    
    if ( t > upper ) {
        upper = t;
        upper_cc = p;
    } 
}

void Interval::updateLower(double t, CCPoint& p) {
    if (lower_cc.type == NONE) {
        //std::cout << " I: updateLower() NONE up=lo=" << t << "\n";
        lower = t;
        upper = t;
        lower_cc = p;
        upper_cc = p;
    }
    
    if ( t < lower ) {
        lower = t; 
        lower_cc = p;
    }
}

/// return true if *this is completely non-overlapping, or outside of i.
bool Interval::outside(const Interval& i) const {
    if ( this->lower > i.upper )
        return true;
    else if ( this->upper < i.lower )
        return true;
    else
        return false;
}

/// return true if *this is contained within i
bool Interval::inside(const Interval& i) const {
    if ( (this->lower > i.lower) && (this->upper < i.upper) )
        return true;
    else
        return false;
}

bool Interval::empty() const {
    if ( (lower==0.0) && (upper==0.0) )
        return true;
    else
        return false;
}

std::string Interval::str() const {
    std::ostringstream o;
    o << "I ["<< lower <<" , " << upper << " ]";
    return o.str();
}


/*********************** Fiber ****************************************/

Fiber::Fiber(const Point &p1in, const Point &p2in) {
    p1=p1in;
    p2=p2in;
    calcDir();
}

void Fiber::calcDir() {
    dir = p2 - p1;
    assert( dir.z == 0.0 );
    dir.normalize();
}

/// return true if some Interval in this Fiber contains i
bool Fiber::contains(Interval& i) const {
    BOOST_FOREACH( Interval fi, ints) {
        if ( i.inside( fi ) )
            return true;
    }
    return false;
}

/// return true if Fiber is completely missing Invterval i
bool Fiber::missing(Interval& i) const {
    bool result = true;
    BOOST_FOREACH( Interval fi, ints) {
        if ( !i.outside( fi ) ) // all existing ints must be non-overlapping
            result = false; 
    }
    return result;
}

void Fiber::addInterval(Interval& i) {
    if (ints.empty()) { // empty fiber case
        ints.push_back(i);
        return;
    } else if ( this->contains(i)  ) { // if fiber already contains i  
        return; // do nothing
    } else if ( this->missing(i) ) { // if fiber doesn't contain i 
        ints.push_back(i);
        return;
    } else {
        // this is the messier general case with partial overlap
        for (unsigned int m=0;m<ints.size();++m) {
            if ( ints[m].lower < i.upper ) {
            }
        }
        return;
    }
    
}


/// return t-value correspoinding to p
double Fiber::tval(Point& p) const {
    // fiber is  f = p1 + t * (p2-p1)
    // t = (f-p1).dot(p2-p1) / (p2-p1).dot(p2-p1)
    return  (p-p1).dot(p2-p1) / (p2-p1).dot(p2-p1);
}

/// return a point on the fiber
Point Fiber::point(double t) const {
    Point p = p1 + t*(p2-p1);
    return p;
}

boost::python::list Fiber::getInts() {
    boost::python::list l;
    BOOST_FOREACH( Interval i, ints) {
        l.append( i );
    }
    return l;
}

void Fiber::printInts() {
    int n=0;
    BOOST_FOREACH( Interval i, ints) {
        std::cout << n << ": [ " << i.lower << " , " << i.upper << " ]" << "\n";
        ++n;
    }
}



} // end namespace
// end file fiber.cpp
