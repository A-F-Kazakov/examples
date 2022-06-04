#include <string>
#include <memory>
#include <iostream>

using namespace std;

struct Implementation
{
	virtual ~Implementation() = default;
	virtual string operation() const = 0;
};

struct ConcreteImplementationA : Implementation
{
	string operation() const override
	{
		return "ConcreteImplementationA: Here's the result on the platform A.\n";
	}
};

struct ConcreteImplementationB : Implementation
{
	string operation() const override
	{
		return "ConcreteImplementationB: Here's the result on the platform B.\n";
	}
};

struct Abstraction
{
	shared_ptr<Implementation> implementation_;

	Abstraction(shared_ptr<Implementation> impl) : implementation_(impl) {}

	virtual ~Abstraction() = default;

	virtual string operation() const
	{
		return "Abstraction: Base operation with:\n" + implementation_->operation();
	}
};

struct ExtendedAbstraction : Abstraction
{
	ExtendedAbstraction(shared_ptr<Implementation> impl) : Abstraction(impl) {}

	string operation() const override
	{
		return "ExtendedAbstraction: Extended operation with:\n" +
           implementation_->operation();
	}
};

int main()
{
	{
		auto impl = make_shared<ConcreteImplementationA>();
		Abstraction obj(impl);

		cout << obj.operation() << endl;
	}

	{
		auto impl = make_shared<ConcreteImplementationB>();
		ExtendedAbstraction obj(impl);

		cout << obj.operation();
	}
}
