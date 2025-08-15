#pragma once
#include "HelperFunctions.h"
#include "Exceptions.h" 
#include "Complex.h"    

template <typename T> class Matrix;

inline std::string toUpper(std::string s)
{
    transform(s.begin(),s.end(),s.begin(),[](unsigned char c){ return toupper(c); });
    return s;
}

class Element
{
protected:
    std::string name;
    std::string node1;
    std::string node2;
    std::string type;
public:
    Element(const std::string& name,const std::string& node1,const std::string& node2,const std::string& type)
        : name(toUpper(name)),node1(toUpper(node1)),node2(toUpper(node2)),type(type)
    {
        if(this->name.empty()||this->node1.empty()||this->node2.empty())
            throw InvalidValueError("Element name and nodes cannot be empty.");
        if(this->node1==this->node2)
            throw InvalidValueError("Element nodes cannot be identical.");
    }

    virtual ~Element()=default;

    virtual std::string toString() const=0;

    const std::string& getName() const { return name; }
    const std::string& getNode1() const { return node1; }
    const std::string& getNode2() const { return node2; }
    const std::string& getType() const { return type; }

    void setNode2(const std::string& newNode) { this->node2=toUpper(newNode); }
    void setNode1(const std::string& newNode) { this->node1=toUpper(newNode); }

    static double parseValue(const std::string& valueStr)
    {
		if(valueStr.empty())
			throw InvalidValueError("Value string cannot be empty.");
		std::map<char,double> multipliers=
		{
			{'F',1e-15}, //Femto
			{'P',1e-12}, //Pico
			{'N',1e-9},  //Nano
			{'U', 1e-6}, // Micro
			{'M', 1e-3}, // Milli
			{'K', 1e3},  // Kilo
			{'G', 1e9},  // Giga
			{'T', 1e12}  // Tera
		};
		double value;
		char suffix='\0';
		std::string numPart;
		size_t i=0;
		while(i<valueStr.length()&&isdigit(valueStr[i])||valueStr[i]=='.'||(i==0&&valueStr[i]=='-'))
		{
			numPart+=valueStr[i];
			i++;
		}
		try { value=stod(numPart); }
		catch(const std::invalid_argument& e){ throw InvalidValueError("Invalid numerical part in value: '"+valueStr+"'"); }
		catch(const std::out_of_range& e) { throw InvalidValueError("Numerical value out of range: '"+valueStr+"'"); }
		if(i<valueStr.length()) //if suffix exists
		{
			suffix=toUpper(std::string(1,valueStr[i]))[0];
			if(multipliers.count(suffix))
				value*=multipliers[suffix];
			else if(suffix=='M'&&i+1<valueStr.length()&&toUpper(std::string(1,valueStr[i+1]))[0]=='E'
				&&i+2<valueStr.length()&&toUpper(std::string(1,valueStr[i+2]))[0]=='G')
				value*=1e6;
			else
				throw InvalidValueError("Unrecognized unit suffix: '"+std::string(1,valueStr[i])+"' in value: '"+valueStr+"'.");
		}
		return value;
    }

    virtual double getInstantaneousValue(double time) const { return 0.0; }

    virtual Complex getComplexAdmittance(double frequency) const { return Complex(0.0,0.0); }

    virtual void stampTransient(Matrix<double>& A,std::vector<double>& b,const std::map<std::string,int>& nodeToIndex,
        const std::map<std::string,int>& voltageSourceNameToCurrentIndex,double dt,double time,
        const std::vector<double>& prev_voltages,const std::vector<double>& prev_branch_currents)=0;

    virtual void updateTransientState(const std::vector<double>& current_voltages,const std::vector<double>& current_branch_currents,
        const std::map<std::string,int>& nodeToIndex,const std::map<std::string,int>& voltageSourceNameToCurrentIndex,double dt) {}

    virtual void initializeTransientState() {}
};