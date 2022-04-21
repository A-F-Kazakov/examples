#include <iostream>
#include <string>
#include <memory>
#include <array>

using namespace std;

struct ConcreteComponentA;
struct ConcreteComponentB;

struct Visitor
{
	virtual ~Visitor() = default;
	virtual void visitConcreteComponentA(const ConcreteComponentA *element) const = 0;
	virtual void visitConcreteComponentB(const ConcreteComponentB *element) const = 0;
};

struct Component
{
	virtual ~Component() noexcept = default;
	virtual void accept(Visitor *visitor) const = 0;
};

struct ConcreteComponentA : Component
{
	void accept(Visitor *visitor) const override { visitor->visitConcreteComponentA(this); }

	string exclusiveMethodOfConcreteComponentA() const { return "A"; }
};

struct ConcreteComponentB : Component
{
	void accept(Visitor *visitor) const override { visitor->visitConcreteComponentB(this); }

	string specialMethodOfConcreteComponentB() const { return "B"; }
};

struct ConcreteVisitor1 : Visitor
{
	void visitConcreteComponentA(const ConcreteComponentA *element) const override {
		cout << element->exclusiveMethodOfConcreteComponentA() << " + ConcreteVisitor1\n";
	}

	void visitConcreteComponentB(const ConcreteComponentB *element) const override {
		cout << element->specialMethodOfConcreteComponentB() << " + ConcreteVisitor1\n";
	}
};

struct ConcreteVisitor2 : Visitor
{
	void visitConcreteComponentA(const ConcreteComponentA *element) const override {
		cout << element->exclusiveMethodOfConcreteComponentA() << " + ConcreteVisitor2\n";
	}

	void visitConcreteComponentB(const ConcreteComponentB *element) const override {
		cout << element->specialMethodOfConcreteComponentB() << " + ConcreteVisitor2\n";
	}
};

void clientCode(array<unique_ptr<Component>, 2> &components, Visitor *visitor)
{
	for(auto& comp : components)
		comp->accept(visitor);
}

int main()
{
	array<unique_ptr<Component>, 2> components{make_unique<ConcreteComponentA>(), make_unique<ConcreteComponentB>()};
	cout << "The client code works with all visitors via the base Visitor interface:\n";
	ConcreteVisitor1 visitor1;
	clientCode(components, &visitor1);

	cout << "\n";
	cout << "It allows the same client code to work with different types of visitors:\n";
	ConcreteVisitor2 visitor2;
	clientCode(components, &visitor2);
}
