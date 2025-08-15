#pragma once
#include <string>
#include <cmath>
#include "Element.h" 
#include "Complex.h" 
#include "Matrix.h"

class Capacitor : public Element
{
private:
    double value; // Capacitance in Farads
    double prev_voltage_diff; // V_node1 - V_node2 at previous time step
    double prev_current_through; // Current through capacitor at previous time step
public:
    // Constructor
    Capacitor(const std::string& name,const std::string& node1,const std::string& node2,const std::string& valueStr)
        : Element(name,node1,node2,"Capacitor"),prev_voltage_diff(0.0),prev_current_through(0.0)
    {
        this->value=parseValue(valueStr);
        if(this->value<=0)
            throw InvalidValueError("Capacitance cannot be zero or negative.");
    }

    // Override toString method
    std::string toString() const override { return "Capacitor "+name+" "+node1+" "+node2+" "+std::to_string(value)+"F"; }

    // Getter for value
    double getValue() const { return value; }

    // Override to get complex admittance for AC analysis
    Complex getComplexAdmittance(double frequency) const override
    {
        double omega=2*M_PI*frequency;
        return J*Complex(0.0,omega*value);
    }

    // Implement stampTransient for capacitors (Trapezoidal Rule)
    void stampTransient(Matrix<double>& A,std::vector<double>& b,const std::map<std::string,int>& nodeToIndex,
        const std::map<std::string,int>& voltageSourceNameToCurrentIndex,double dt,double time,
        const std::vector<double>& prev_voltages,const std::vector<double>& prev_branch_currents) override
    {
        (void)time;
        (void)prev_branch_currents;
        (void)voltageSourceNameToCurrentIndex;
        if(dt<=0)
            throw CircuitError("Time step (dt) must be positive for transient analysis.");
        double G_eq=2.0*value/dt; // Equivalent conductance
        // Calculate I_history(t_{n-1})
        // This is the current source part of the companion model
        double I_history=G_eq*prev_voltage_diff+prev_current_through;
        int idx1=(node1=="GND")?-1:nodeToIndex.at(node1);
        int idx2=(node2=="GND")?-1:nodeToIndex.at(node2);
        // Stamp G_eq into the MNA matrix
        if(idx1!=-1)
            A.add(idx1,idx1,G_eq);
        if(idx2!=-1)
            A.add(idx2,idx2,G_eq);
        if(idx1!=-1&&idx2!=-1)
        {
            A.add(idx1,idx2,-G_eq);
            A.add(idx2,idx1,-G_eq);
        }

        // Stamp I_history into the RHS vector
        // If I_history is defined such that I_C(t_n) = G_eq * V_C(t_n) - I_history,
        // then current I_history flows into node1 and out of node2.
        // So for KCL at node1 (sum of currents leaving): -I_history is added to RHS.
        // For KCL at node2 (sum of currents leaving): +I_history is added to RHS.
        // This corresponds to I_history as a source FROM node2 TO node1.
        if(idx1!=-1)
            b[idx1]+=I_history;
        if(idx2!=-1)
            b[idx2]-=I_history;
    }

    // Override to update internal state after a transient step
    void updateTransientState(const std::vector<double>& current_voltages,const std::vector<double>& current_branch_currents,
        const std::map<std::string,int>& nodeToIndex,const std::map<std::string,int>& voltageSourceNameToCurrentIndex,double dt) override
    {
        (void)current_branch_currents;
        (void)voltageSourceNameToCurrentIndex;
        int idx1=(node1=="GND")?-1:nodeToIndex.at(node1);
        int idx2=(node2=="GND")?-1:nodeToIndex.at(node2);
        double V1=(idx1!=-1)?current_voltages[idx1]:0.0;
        double V2=(idx2!=-1)?current_voltages[idx2]:0.0;
        double current_voltage_diff=V1-V2; // This is V_C(n)
        double old_prev_voltage_diff=prev_voltage_diff;
        double old_prev_current_through=prev_current_through;
        prev_voltage_diff=current_voltage_diff;
        // Calculation of current through capacitor at current step (I_C(n))
        // I_C(n) = (2C/dt) * (V_C(n) - V_C(n-1)) - I_C(n-1)
        prev_current_through=(2.0*value/dt)*(current_voltage_diff-old_prev_voltage_diff)-old_prev_current_through;
    }

    // Override to initialize transient state
    void initializeTransientState() override
    {
        prev_voltage_diff=0.0;
        prev_current_through=0.0;
    }
};