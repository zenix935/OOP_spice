#pragma once
#include "Element.h" 
#include "Complex.h"    

class Resistor : public Element
{
private:
    double value; // Resistance in Ohms 
public:
    Resistor(const std::string& name,const std::string& node1,const std::string& node2,const std::string& valueStr)
        :Element(name,node1,node2,"Resistor")
    {
        this->value=parseValue(valueStr);
        if(this->value<=0) // Resistance should be positive
            throw InvalidValueError("Resistance cannot be zero or negative.");
    }

    // Override toString method
    std::string toString() const override { return "Resistor "+name+" "+node1+" "+node2+" "+std::to_string(value)+"Ohm"; }

    // Getter for value
    double getValue() const { return value; }

    // Override to get complex admittance for AC analysis
    Complex getComplexAdmittance(double frequency) const override
    {
        // Admittance of a resistor is simply 1/R (real part only)
        return Complex(1.0/value,0.0);
    }

    // Resistors do not have a time-dependent companion model for transient analysis
    // beyond their simple conductance, which is handled directly in Circuit::solveTransient.
    // This method is intentionally empty to acknowledge it's not needed for this component.
    void stampTransient(Matrix<double>& A,std::vector<double>& b,const std::map<std::string,int>& nodeToIndex,
        const std::map<std::string,int>& voltageSourceNameToCurrentIndex,double dt,double time,
        const std::vector<double>& prev_voltages,const std::vector<double>& prev_branch_currents) override
    {
        // Resistors are handled by their direct conductance stamping in Circuit::solveTransient.
        // The parameters are marked (void) to suppress unused variable warnings.
        (void)A; (void)b; (void)nodeToIndex; (void)voltageSourceNameToCurrentIndex; (void)dt; (void)time; (void)prev_voltages; (void)prev_branch_currents;
    }
};