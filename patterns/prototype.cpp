#include <iostream>
#include <string>
#include <memory>
#include <unordered_map>

using namespace std;

enum Type
{
  prototype_1,
  prototype_2
};

struct Prototype
{
	Prototype() = default;
	Prototype(string prototype_name) : prototype_name_(prototype_name) {}

	virtual ~Prototype() = default;
	virtual unique_ptr<Prototype> clone() const = 0;
	virtual void method(float prototype_field)
	{
		prototype_field_ = prototype_field;
		cout << "Call Method from " << prototype_name_ << " with field : " << prototype_field << endl;
	}

	protected:
		string prototype_name_;
		float prototype_field_{};
};

struct ConcretePrototype1 : Prototype
{
	ConcretePrototype1(string prototype_name, float concrete_prototype_field) :
		Prototype(prototype_name), concrete_prototype_field1_(concrete_prototype_field)
	{}

	unique_ptr<Prototype> clone() const override { return make_unique<ConcretePrototype1>(*this); }

	private:
		float concrete_prototype_field1_;
};

struct ConcretePrototype2 : Prototype
{
	ConcretePrototype2(string prototype_name, float concrete_prototype_field) :
		Prototype(prototype_name), concrete_prototype_field2_(concrete_prototype_field)
	{}

	unique_ptr<Prototype> clone() const override { return make_unique<ConcretePrototype2>(*this); }

	private:
		float concrete_prototype_field2_;
};

struct PrototypeFactory
{
	PrototypeFactory()
	{
		prototypes_[Type::prototype_1] = make_unique<ConcretePrototype1>("PROTOTYPE_1 ", 50.f);
		prototypes_[Type::prototype_2] = make_unique<ConcretePrototype2>("PROTOTYPE_2 ", 60.f);
	}

	unique_ptr<Prototype> createPrototype(Type type) { return prototypes_[type]->clone(); }

	private:
		unordered_map<Type, unique_ptr<Prototype>, hash<int>> prototypes_;
};

int main()
{
	PrototypeFactory prototype_factory;

	cout << "Let's create a Prototype 1\n";

	auto prototype = prototype_factory.createPrototype(Type::prototype_1);
	prototype->method(90);

	cout << "\n";

	cout << "Let's create a Prototype 2 \n";

	prototype = prototype_factory.createPrototype(Type::prototype_2);
	prototype->method(10);
}
