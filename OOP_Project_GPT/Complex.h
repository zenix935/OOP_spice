#pragma once
#include <cmath>  
#include <iostream> 
#include <iomanip>  
#include <limits>  

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class Complex
{
private:
    double real;
    double imag;
public:
    Complex(double r=0.0,double i=0.0) : real(r),imag(i) {}

    double getReal() const { return real; }
    double getImag() const { return imag; }

    double magnitude() const { return sqrt(real*real+imag*imag); }

    double angleRadians() const { return atan2(imag,real); }

    double angleDegrees() const { return angleRadians()*180.0/M_PI; }

    Complex conjugate() const { return Complex(real,-imag); }

    Complex operator+(const Complex& other) const { return Complex(real+other.real,imag+other.imag); }
    Complex operator-(const Complex& other) const { return Complex(real-other.real,imag-other.imag); }
    Complex operator*(const Complex& other) const
    {
        return Complex(real*other.real-imag*other.imag,
            real*other.imag+imag*other.real);
    }
    Complex operator/(const Complex& other) const
    {
        double denom=other.real*other.real+other.imag*other.imag;
        if(denom==0.0)
            return Complex(std::numeric_limits<double>::infinity(),std::numeric_limits<double>::infinity());
        return Complex((real*other.real+imag*other.imag)/denom,
            (imag*other.real-real*other.imag)/denom);
    }
    Complex& operator+=(const Complex& other)
    {
        real+=other.real;
        imag+=other.imag;
        return *this;
    }
    Complex& operator-=(const Complex& other)
    {
        real-=other.real;
        imag-=other.imag;
        return *this;
    }
    Complex& operator*=(const Complex& other)
    {
        double newReal=real*other.real-imag*other.imag;
        double newImag=real*other.imag+imag*other.real;
        real=newReal;
        imag=newImag;
        return *this;
    }
    Complex& operator/=(const Complex& other)
    {
        double denom=other.real*other.real+other.imag*other.imag;
        if(denom==0.0)
        {
            real=std::numeric_limits<double>::infinity();
            imag=std::numeric_limits<double>::infinity();
            return *this;
        }
        double newReal=(real*other.real+imag*other.imag)/denom;
        double newImag=(imag*other.real-real*other.imag)/denom;
        real=newReal;
        imag=newImag;
        return *this;
    }

    Complex operator-() const { return Complex(-real,-imag); }

    bool operator==(const Complex& other) const { return real==other.real&&imag==other.imag; }
    bool operator!=(const Complex& other) const { return !(*this==other); }

    friend std::ostream& operator<<(std::ostream& os,const Complex& c)
    {
        os<<std::fixed<<std::setprecision(4);
        if(c.imag==0)
            os<<c.real;
        else if(c.real==0)
            os<<c.imag<<"j";
        else
            os<<c.real<<(c.imag>0?" + ":" - ")<<abs(c.imag)<<"j";
        return os;
    }

    friend Complex operator*(double val,const Complex& c) { return Complex(val*c.real,val*c.imag); }
    friend Complex operator*(const Complex& c,double val) { return Complex(val*c.real,val*c.imag); }

    friend Complex operator/(double val,const Complex& c)
    {
        double denom=c.real*c.real+c.imag*c.imag;
        if(denom==0.0)
            return Complex(std::numeric_limits<double>::infinity(),std::numeric_limits<double>::infinity());
        return Complex(val*c.real/denom,-val*c.imag/denom);
    }
};

const Complex J(0.0,1.0);

inline double abs(const Complex& c) { return c.magnitude(); }