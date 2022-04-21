#include <iostream>
#include <string>
#include <memory>

using namespace std;

struct Implementation
{
	virtual ~Implementation() = default;
	virtual string operationImplementation() const = 0;
};

struct ConcreteImplementationA : Implementation
{
	string operationImplementation() const override { return "ConcreteImplementationA: Here's the result on the platform A.\n"; }
};

struct ConcreteImplementationB : Implementation
{
	string operationImplementation() const override { return "ConcreteImplementationB: Here's the result on the platform B.\n"; }
};

struct Abstraction
{
	Abstraction(Implementation* implementation) : implementation_(implementation) {}

  virtual ~Abstraction() = default;

  virtual string operation() const { return "Abstraction: Base operation with:\n" + implementation_->operationImplementation(); }

	protected:
		Implementation* implementation_;
};

struct ExtendedAbstraction : Abstraction
{
	using Abstraction::Abstraction;

  string operation() const override { return "ExtendedAbstraction: Extended operation with:\n" + implementation_->operationImplementation(); }
};

int main()
{
	ConcreteImplementationA implementation;
	Abstraction abstraction(&implementation);
	cout << abstraction.operation();
	cout << endl;

	ConcreteImplementationB implementation2;
	ExtendedAbstraction exAbstraction(&implementation);
	cout << exAbstraction.operation();
}
