#include <iostream>
#include <string>
#include <memory>
#include <algorithm>

using namespace std;

struct Target
{
	virtual ~Target() = default;
	virtual string request() const { return "Target: The default target's behavior."; }
};

struct Adaptee
{
	string specificRequest() const { return ".eetpadA eht fo roivaheb laicepS"; }
};

struct Adapter : Target
{
	Adapter(unique_ptr<Adaptee>&& adaptee) : adaptee_(move(adaptee)) {}

	string request() const override
	{
		auto to_reverse = adaptee_->specificRequest();
		reverse(to_reverse.begin(), to_reverse.end());
		return "Adapter: (TRANSLATED) " + to_reverse;
	}

	private:
		unique_ptr<Adaptee> adaptee_;
};

int main()
{
	cout << "Client: I can work just fine with the Target objects:\n";
	Target target;
	cout << target.request();

	cout << "\n\n";
	auto adaptee = make_unique<Adaptee>();
	cout << "Client: The Adaptee class has a weird interface. See, I don't understand it:\n";
	cout << "Adaptee: " << adaptee->specificRequest();
	cout << "\n\n";
	cout << "Client: But I can work with it via the Adapter:\n";
	Adapter adapter{move(adaptee)};
	cout << target.request();
	cout << "\n";
}
