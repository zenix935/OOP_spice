#pragma once
#include "Element.h" 
#include "Complex.h"
#include "Matrix.h"

class Inductor : public Element
{
private:
    double value; // Inductance in Henries
    double prev_current; // Current through inductor at previous time step (from node1 to node2)
    double prev_voltage_across; // Voltage across inductor at previous time step (V(node1) - V(node2))
public:
    // Constructor
    Inductor(const std::string& name,const std::string& node1,const std::string& node2,const std::string& valueStr)
        : Element(name,node1,node2,"Inductor"),prev_current(0.0),prev_voltage_across(0.0)
    {
        this->value=parseValue(valueStr);
        if(this->value<=0)
            throw InvalidValueError("Inductance cannot be zero or negative.");
    }

    // Override toString method
    std::string toString() const override { return "Inductor "+name+" "+node1+" "+node2+" "+std::to_string(value)+"H"; }

    // Getter for value
    double getValue() const { return value; }

    // Override to get complex admittance for AC analysis
    Complex getComplexAdmittance(double frequency) const override
    {
        // Admittance of an inductor is 1 / (j * omega * L) = -j / (omega * L)
        double omega=2*M_PI*frequency;
        if(omega==0) // Handle DC case (short circuit for voltage constraint)
            return Complex(0.0,0.0); // An ideal inductor is a short circuit at DC (0 frequency)
        return Complex(0.0,-1.0/(omega*value));
    }

    // Implement stampTransient for inductors (Trapezoidal Rule)
    void stampTransient(Matrix<double>& A,std::vector<double>& b,const std::map<std::string,int>& nodeToIndex,
        const std::map<std::string,int>& voltageSourceNameToCurrentIndex,double dt,double time,
        const std::vector<double>& prev_voltages,const std::vector<double>& prev_branch_currents) override
    {
        (void)time;
        (void)prev_voltages;
        (void)prev_branch_currents;
        if(dt<=0)
            throw CircuitError("Time step (dt) must be positive for transient analysis.");

        // Find the index of the inductor's branch current in the MNA matrix/vector
        auto it=voltageSourceNameToCurrentIndex.find(name);
        if(it==voltageSourceNameToCurrentIndex.end())
            throw CircuitError("Inductor '"+name+"' does not have a registered branch current index for transient analysis.");
        int branchIdx=it->second;
        // Calculate the equivalent resistance (R_eq) for the trapezoidal companion model
        double R_eq=2.0*value/dt;
        // Calculate the equivalent voltage source (V_eq) for the history term.
        // V_eq = V_L(n-1) + R_eq * I_L(n-1)
        double V_eq=prev_voltage_across+R_eq*prev_current;
        // Get the global indices for the component's nodes, handling GND (index -1)
        int idx1=(node1=="GND")?-1:nodeToIndex.at(node1);
        int idx2=(node2=="GND")?-1:nodeToIndex.at(node2);
        // Stamp the MNA matrix (A) coefficients related to the inductor
        // The branch voltage constraint equation (row for branchIdx) is: V_n1 - V_n2 - R_eq * I_L = -V_eq
        // Or, (V_n1 - V_n2) - R_eq * I_L + V_eq = 0
        // So, in A*x=b form, the V_eq term goes to the RHS as -V_eq.
        // Add coefficients for V_node1 and V_node2
        if(idx1!=-1)
            A.add(branchIdx,idx1,1.0);
        if(idx2!=-1)
            A.add(branchIdx,idx2,-1.0);
        // Add coefficient for the inductor's branch current (I_L)
        A.add(branchIdx,branchIdx,-R_eq); // Coefficient for inductor current variable
        // Stamp the RHS vector (b)
        // The V_eq term goes to the RHS with a negative sign.
        b[branchIdx]-=V_eq;
        // KCL equations (rows corresponding to nodes)
        // Inductor branch current (I_L) variable is involved in KCL equations at its nodes.
        // I_L flows from node1 to node2.
        if(idx1!=-1)
            A.add(idx1,branchIdx,1.0); // +1 coefficient for branch current leaving n1
        if(idx2!=-1)
            A.add(idx2,branchIdx,-1.0); // -1 coefficient for branch current entering n2
    }

    // Override to update internal state after a transient step
    void updateTransientState(const std::vector<double>& current_voltages,const std::vector<double>& current_branch_currents,
        const std::map<std::string,int>& nodeToIndex,const std::map<std::string,int>& voltageSourceNameToCurrentIndex,double dt) override
    {
        (void)nodeToIndex;
        (void)dt;
        auto it=voltageSourceNameToCurrentIndex.find(name);
        if(it==voltageSourceNameToCurrentIndex.end())
            // This should not happen if stampTransient succeeded.
            // If it does, consider adding more robust error handling or logging.
            return;

        int branchIdx=it->second;
        // The current through the inductor is the solved branch current (I_L(n))
        double current_I=current_branch_currents[branchIdx];
        // The voltage across the inductor at the current step (V_L(n))
        int idx1=(node1=="GND")?-1:nodeToIndex.at(node1);
        int idx2=(node2=="GND")?-1:nodeToIndex.at(node2);
        double V1=(idx1!=-1)?current_voltages[idx1]:0.0;
        double V2=(idx2!=-1)?current_voltages[idx2]:0.0;
        double current_V_across=V1-V2;
        // Update previous values with the current step's solved values.
        // These become I_L(n-1) and V_L(n-1) for the *next* time step's calculations.
        prev_current=current_I;
        prev_voltage_across=current_V_across;
    }

    // Override to initialize transient state
    void initializeTransientState() override
    {
        prev_current=0.0;
        prev_voltage_across=0.0;
    }
};