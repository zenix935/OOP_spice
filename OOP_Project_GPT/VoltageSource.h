#pragma once
#include "Element.h" 
#include "Complex.h"      

class VoltageSource : public Element
{
private:
    double dc_value;     // DC voltage in Volts
    double ac_magnitude; // AC magnitude in Volts
    double ac_phase;     // AC phase in degrees
    double ac_frequency; // AC frequency in Hz (0 for DC source, or if not an AC source)

    // Helper to check if it's an AC source
    bool isACSource() const { return ac_magnitude>0||ac_phase!=0||ac_frequency>0; }
public:
    // Constructor for DC Voltage Source
    VoltageSource(const std::string& name,const std::string& node1,const std::string& node2,const std::string& dcValueStr)
        :Element(name,node1,node2,"VoltageSource"), // Changed base class constructor call
        dc_value(parseValue(dcValueStr)),ac_magnitude(0.0),ac_phase(0.0),ac_frequency(0.0) {}

    // Constructor for AC Voltage Source (magnitude, phase, frequency)
    VoltageSource(const std::string& name,const std::string& node1,const std::string& node2,
        const std::string& acMagnitudeStr,const std::string& acPhaseStr,const std::string& acFrequencyStr)
        : Element(name,node1,node2,"VoltageSource"),dc_value(0.0)
    {
        this->ac_magnitude=parseValue(acMagnitudeStr);
        this->ac_phase=parseValue(acPhaseStr);
        this->ac_frequency=parseValue(acFrequencyStr);
        if(this->ac_magnitude<0)
            throw InvalidValueError("AC magnitude for voltage source cannot be negative.");
        if(this->ac_frequency<0) // Frequency usually positive, 0 for DC equivalent
            throw InvalidValueError("AC frequency for voltage source cannot be negative.");
    }

    // Override toString method
    std::string toString() const override
    {
        if(isACSource())
            return "VoltageSource "+name+" "+node1+" "+node2+" AC Mag="+std::to_string(ac_magnitude)+"V Phase="+std::to_string(ac_phase)+"deg Freq="+std::to_string(ac_frequency)+"Hz";
        else
            return "VoltageSource "+name+" "+node1+" "+node2+" DC="+std::to_string(dc_value)+"V";
    }

    // Getters for DC/AC values
    double getDCValue() const { return dc_value; }
    void setDCValue(double new_val) { dc_value=new_val; } // For DC sweep

    // Get the phasor for AC analysis at a given frequency
    Complex getACPhasor(double analysisFrequency) const
    {
        if(isACSource()&&ac_frequency==analysisFrequency)
        {   // Convert magnitude and phase to complex number
            double phase_rad=ac_phase*M_PI/180.0;
            return Complex(ac_magnitude*cos(phase_rad),ac_magnitude*sin(phase_rad));
        }
        else if(ac_frequency==0.0&&analysisFrequency==0.0) // DC analysis, treat as DC value with 0 phase
            return Complex(dc_value,0.0);
        else
        {
            // If source frequency doesn't match analysis frequency, it's a short circuit (0 voltage)
            // or if it's a DC source and AC analysis is being run.
            return Complex(0.0,0.0);
        }
    }

    // Get instantaneous value for transient analysis
    double getInstantaneousValue(double time) const override
    {
        if(isACSource())
        {   // For AC source, instantaneous value is magnitude * sin(omega*t + phase)
            double omega=2*M_PI*ac_frequency;
            double phase_rad=ac_phase*M_PI/180.0;
            return ac_magnitude*sin(omega*time+phase_rad);
        }
        else  // For DC source, instantaneous value is just the DC value
            return dc_value;
    }

    // Voltage sources do not have admittance for MNA (they add a row/column for branch current)
    Complex getComplexAdmittance(double frequency) const override
    {
        // A voltage source is ideally a short circuit with a controlled voltage.
        // Its admittance is theoretically infinite, but for MNA, it's handled
        // by adding a branch current variable, not by stamping an admittance.
        return Complex(0.0,0.0);
    }

    // Independent Voltage Sources add a branch current variable.
    // The stampTransient method handles their contribution to the MNA matrix and RHS.
    // Independent Voltage Sources are handled directly by the Circuit::solveTransient method
    // because they introduce a new row/column in the MNA matrix.
    void stampTransient(Matrix<double>& A,std::vector<double>& b,const std::map<std::string,int>& nodeToIndex,
        const std::map<std::string,int>& voltageSourceNameToCurrentIndex,double dt,double time,
        const std::vector<double>& prev_voltages,const std::vector<double>& prev_branch_currents) override
    {
        // This method is intentionally empty for VoltageSource, as its contribution
        // to 'A' and 'b' is handled directly in Circuit::solveTransient based on its instantaneous value.
        // The parameters are marked (void) to suppress unused variable warnings.
        (void)A; (void)b; (void)nodeToIndex; (void)voltageSourceNameToCurrentIndex; (void)dt; (void)time; (void)prev_voltages; (void)prev_branch_currents;
    }// Handled directly in Circuit::solveTransient
};