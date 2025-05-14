#include <iostream>
#include <list>
#include <memory>

using namespace std;

struct Component : enable_shared_from_this<Component>
{
	virtual ~Component() = default;
	void setParent(shared_ptr<Component> parent) { parent_ = parent; }
	shared_ptr<Component> getParent() const { return parent_; }

	virtual void add(shared_ptr<Component>) {}
	virtual void remove(shared_ptr<Component>) {}
	virtual bool isComposite() const { return false; }
	virtual string operation() const = 0;

	protected:
	shared_ptr<Component> parent_;
};

struct Leaf : Component
{
	string operation() const override { return "Leaf"; }
};

struct Composite : Component
{
	void add(shared_ptr<Component> component) override
	{
		children_.push_back(component);
		component->setParent(shared_from_this());
	}

	void remove(shared_ptr<Component> component) override
	{
		children_.remove(component);
		component->setParent(nullptr);
	}

	bool isComposite() const override { return true; }

	string operation() const override
	{
		string result;
		for(const auto &c : children_)
			if(c == children_.back())
				result += c->operation();
			else
				result += c->operation() + "+";

		return "Branch(" + result + ")";
	}

	protected:
		list<shared_ptr<Component>> children_;
};

void clientCode(shared_ptr<Component> component) { cout << "RESULT: " << component->operation(); }

void clientCode2(shared_ptr<Component> component1, shared_ptr<Component> component2)
{
	if(component1->isComposite())
		component1->add(component2);

	clientCode(component1);
}

int main()
{
	auto simple = make_shared<Leaf>();
	cout << "Client: I've got a simple component:\n";
	clientCode(simple);
	cout << "\n\n";

	auto tree = make_shared<Composite>();
	auto branch1 = make_shared<Composite>();

	auto leaf_1 = make_shared<Leaf>();
	auto leaf_2 = make_shared<Leaf>();
	auto leaf_3 = make_shared<Leaf>();
	branch1->add(leaf_1);
	branch1->add(leaf_2);
	auto branch2 = make_shared<Composite>();
	branch2->add(leaf_3);
	tree->add(branch1);
	tree->add(branch2);
	cout << "Client: Now I've got a composite tree:\n";
	clientCode(tree);
	cout << "\n\n";

	cout << "Client: I don't need to check the components classes even when managing the tree:\n";
	clientCode2(tree, simple);
	cout << "\n";
}
