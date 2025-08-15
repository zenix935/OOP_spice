#pragma once
#include "Element.h" 
#include "Complex.h"

class CurrentSource : public Element
{
private:
    double dc_value;     // DC current in Amps
    double ac_magnitude; // AC magnitude in Amps
    double ac_phase;     // AC phase in degrees
    double ac_frequency; // AC frequency in Hz (0 for DC source, or if not an AC source)

    // Helper to check if it's an AC source
    bool isACSource() const { return ac_magnitude>0||ac_phase!=0||ac_frequency>0; }
public:
    // Constructor for DC Current Source
    CurrentSource(const std::string& name,const std::string& node1,const std::string& node2,const std::string& dcValueStr)
        : Element(name,node1,node2,"CurrentSource"),dc_value(parseValue(dcValueStr)),
        ac_magnitude(0.0),ac_phase(0.0),ac_frequency(0.0) {}

    // Constructor for AC Current Source (magnitude, phase, frequency)
    CurrentSource(const std::string& name,const std::string& node1,const std::string& node2,
        const std::string& acMagnitudeStr,const std::string& acPhaseStr,const std::string& acFrequencyStr)
        : Element(name,node1,node2,"CurrentSource"),dc_value(0.0)
    {   // AC sources have 0 DC value initially
        this->ac_magnitude=parseValue(acMagnitudeStr);
        this->ac_phase=parseValue(acPhaseStr);
        this->ac_frequency=parseValue(acFrequencyStr);
        if(this->ac_magnitude<0)
            throw InvalidValueError("AC magnitude for current source cannot be negative.");
        if(this->ac_frequency<0) // Frequency usually positive, 0 for DC equivalent
            throw InvalidValueError("AC frequency for current source cannot be negative.");
    }

    // Override toString method
    std::string toString() const override
    {
        if(isACSource())
            return "CurrentSource "+name+" "+node1+" "+node2+" AC Mag="+std::to_string(ac_magnitude)
            +"A Phase="+std::to_string(ac_phase)+"deg Freq="+std::to_string(ac_frequency)+"Hz";
        else
            return "CurrentSource "+name+" "+node1+" "+node2+" DC="+std::to_string(dc_value)+"A";
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
            return Complex(ac_magnitude*std::cos(phase_rad),ac_magnitude*std::sin(phase_rad));
        }
        else if(ac_frequency==0.0&&analysisFrequency==0.0)  // DC analysis, treat as DC value with 0 phase
            return Complex(dc_value,0.0);
        else
            // If source frequency doesn't match analysis frequency, it's a short circuit (0 current)
            // or if it's a DC source and AC analysis is being run.
            return Complex(0.0,0.0);
    }

    // Get instantaneous value for transient analysis
    double getInstantaneousValue(double time) const override
    {
        if(isACSource())
        {   // For AC source, instantaneous value is magnitude * sin(omega*t + phase)
            double omega=2*M_PI*ac_frequency;
            double phase_rad=ac_phase*M_PI/180.0;
            return ac_magnitude*std::sin(omega*time+phase_rad);
        }
        else // For DC source, instantaneous value is just the DC value
            return dc_value;
    }

    // Current sources do not have admittance for MNA (they are treated as current injection)
    Complex getComplexAdmittance(double frequency) const override { return Complex(0.0,0.0); } // No direct admittance for current sources
    // Independent Current Sources contribute to the RHS vector, not directly to the A matrix via stamp.
    // The Circuit::stampTransient method will handle their contribution to 'b'.
    void stampTransient(Matrix<double>& A,std::vector<double>& b,const std::map<std::string,int>& nodeToIndex,
        const std::map<std::string,int>& voltageSourceNameToCurrentIndex,double dt,double time,
        const std::vector<double>& prev_voltages,const std::vector<double>& prev_branch_currents) override
    {
        // This method is intentionally empty for CurrentSource, as its contribution
        // to 'b' is handled directly in Circuit::solveTransient based on its instantaneous value.
        // The parameters are marked (void) to suppress unused variable warnings.
        (void)A; (void)b; (void)nodeToIndex; (void)voltageSourceNameToCurrentIndex; (void)dt; (void)time; (void)prev_voltages; (void)prev_branch_currents;
    }
};