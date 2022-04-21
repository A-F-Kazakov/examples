#include <iostream>
#include <string>
#include <memory>

using namespace std;

struct Subject
{
	virtual ~Subject() = default;
	virtual void request() const = 0;
};

struct ConcreteSubject : Subject
{
	void request() const override { cout << "RealSubject: Handling request.\n"; }
};

struct Proxy : Subject
{
	Proxy(unique_ptr<Subject>&& subj) : subj_(move(subj)) {}

	void request() const override
	{
		if(!checkAccess())
			return;

		subj_->request();
		logAccess();
	}

	bool checkAccess() const
	{
		cout << "Proxy: Checking access prior to firing a real request.\n";
		return true;
	}

	void logAccess() const { cout << "Proxy: Logging the time of request.\n"; }

	private:
		unique_ptr<Subject> subj_;
};

int main()
{
	cout << "Client: Executing the client code with a real subject:\n";
	auto subj = make_unique<ConcreteSubject>();
	subj->request();

	cout << "\n";
	cout << "Client: Executing the same client code with a proxy:\n";
	Proxy proxy{move(subj)};
	proxy.request();
}
