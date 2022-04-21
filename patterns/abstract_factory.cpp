#include <iostream>
#include <string>
#include <memory>

using namespace std;

struct AbstractProductA
{
  virtual ~AbstractProductA() = default;
  virtual string usefulFunctionA() const = 0;
};

struct ConcreteProductA1 : AbstractProductA
{
	string usefulFunctionA() const override { return "The result of the product A1."; }
};

struct ConcreteProductA2 : AbstractProductA
{
	string usefulFunctionA() const override { return "The result of the product A2."; }
};

struct AbstractProductB
{
	virtual ~AbstractProductB() = default;
	virtual string usefulFunctionB() const = 0;
	virtual string anotherUsefulFunctionB(const AbstractProductA &collaborator) const = 0;
};

struct ConcreteProductB1 : AbstractProductB
{
	string usefulFunctionB() const override { return "The result of the product B1."; }
	string anotherUsefulFunctionB(const AbstractProductA &collaborator) const override
	{
		const string result = collaborator.usefulFunctionA();
		return "The result of the B1 collaborating with ( " + result + " )";
	}
};

struct ConcreteProductB2 : AbstractProductB
{
	string usefulFunctionB() const override { return "The result of the product B2."; }
	string anotherUsefulFunctionB(const AbstractProductA &collaborator) const override
	{
		const string result = collaborator.usefulFunctionA();
		return "The result of the B2 collaborating with ( " + result + " )";
	}
};

struct AbstractFactory
{
	virtual ~AbstractFactory() = default;
	virtual unique_ptr<AbstractProductA> createProductA() const = 0;
	virtual unique_ptr<AbstractProductB> createProductB() const = 0;
};

struct ConcreteFactory1 : AbstractFactory
{
	unique_ptr<AbstractProductA> createProductA() const override { return make_unique<ConcreteProductA1>(); }
	unique_ptr<AbstractProductB> createProductB() const override { return make_unique<ConcreteProductB1>(); }
};

struct ConcreteFactory2 : AbstractFactory
{
	unique_ptr<AbstractProductA> createProductA() const override { return make_unique<ConcreteProductA2>(); }
	unique_ptr<AbstractProductB> createProductB() const override { return make_unique<ConcreteProductB2>(); }
};

void clientCode(const AbstractFactory &factory)
{
	const auto product_a = factory.createProductA();
	const auto product_b = factory.createProductB();
	cout << product_b->usefulFunctionB() << "\n";
	cout << product_b->anotherUsefulFunctionB(*product_a.get()) << "\n";
}

int main()
{
	cout << "Client: Testing client code with the first factory type:\n";

	ConcreteFactory1 f1;
	clientCode(f1);

	cout << endl;
	cout << "Client: Testing the same client code with the second factory type:\n";
	ConcreteFactory2 f2;
	clientCode(f2);
}
