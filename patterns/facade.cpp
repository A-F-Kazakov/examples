#include <iostream>
#include <string>

using namespace std;

struct Subsystem1
{
	string operation1() const { return "Subsystem1: Ready!\n"; }
	string operationN() const { return "Subsystem1: Go!\n"; }
};

struct Subsystem2
{
	string operation1() const { return "Subsystem2: Get ready!\n"; }
	string operationZ() const { return "Subsystem2: Fire!\n"; }
};

struct Facade
{
	Facade(Subsystem1 *subsystem1, Subsystem2 *subsystem2) : subsystem1_(subsystem1), subsystem2_(subsystem2) {}

	string operation()
	{
		string result = "Facade initializes subsystems:\n";

		if(subsystem1_ != nullptr)
			result += subsystem1_->operation1();

		if(subsystem2_ != nullptr)
			result += subsystem2_->operation1();

		result += "Facade orders subsystems to perform the action:\n";

		if(subsystem1_ != nullptr)
			result += subsystem1_->operationN();

		if(subsystem2_ != nullptr)
			result += subsystem2_->operationZ();

		return result;
	}

	protected:
		Subsystem1 *subsystem1_;
		Subsystem2 *subsystem2_;
};

int main()
{
	Subsystem1 subsystem1;
	Subsystem2 subsystem2;

	Facade facade(&subsystem1, &subsystem2);

	cout << facade.operation();
}
