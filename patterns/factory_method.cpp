#include <iostream>
#include <string>
#include <memory>

using namespace std;

struct Product
{
	virtual ~Product() = default;
	virtual string operation() const = 0;
};

struct ConcreteProduct1 : Product
{
	string operation() const override { return "{Result of the ConcreteProduct1}"; }
};

struct ConcreteProduct2 : Product
{
	string operation() const override { return "{Result of the ConcreteProduct2}"; }
};

struct Creator
{
	virtual ~Creator() = default;
	virtual unique_ptr<Product> factoryMethod() const = 0;

	string someOperation() const
	{
		auto product = factoryMethod();
		return "Creator: The same creator's code has just worked with " + product->operation();
	}
};

struct ConcreteCreator1 : Creator
{
	unique_ptr<Product> factoryMethod() const override { return make_unique<ConcreteProduct1>(); }
};

struct ConcreteCreator2 : Creator
{
	unique_ptr<Product> factoryMethod() const override { return make_unique<ConcreteProduct2>(); }
};

int main()
{
	cout << "App: Launched with the ConcreteCreator1.\n";
	ConcreteCreator1 creator;
	cout << "Client: I'm not aware of the creator's class, but it still works.\n" << creator.someOperation() << endl;
	cout << endl;

	cout << "App: Launched with the ConcreteCreator2.\n";
	ConcreteCreator2 creator2;
	cout << "Client: I'm not aware of the creator's class, but it still works.\n" << creator.someOperation() << endl;
}
