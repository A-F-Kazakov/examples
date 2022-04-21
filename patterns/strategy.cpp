#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>

using namespace std;

struct Strategy
{
	virtual ~Strategy() = default;
	virtual string doAlgorithm(const vector<string> &data) const = 0;
};

struct Context
{
	Context(Strategy *strategy = nullptr) : strategy_(strategy) {}

	void setStrategy(Strategy *strategy) { strategy_.reset(strategy); }

	void doSomeBusinessLogic() const
	{
		cout << "Context: Sorting data using the strategy (not sure how it'll do it)\n";
		string result = strategy_->doAlgorithm({"a", "e", "c", "b", "d"});
		cout << result << "\n";
	}

	private:
		unique_ptr<Strategy> strategy_;
};

struct ConcreteStrategyA : Strategy
{
	string doAlgorithm(const vector<string> &data) const override
	{
		string result;
		for(auto &el : data)
			result += el;

		sort(begin(result), end(result));

		return result;
	}
};

struct ConcreteStrategyB : Strategy
{
	string doAlgorithm(const vector<string> &data) const override
	{
		string result;
		for(auto &el : data)
			result += el;
		sort(begin(result), end(result));
		auto b = result.begin();
		auto e = result.end() - 1;

		while(b != e)
			swap(*(b++), *(e--));

		return result;
	}
};

int main()
{
	Context context{new ConcreteStrategyA};
	cout << "Client: Strategy is set to normal sorting.\n";
	context.doSomeBusinessLogic();
	cout << "\n";
	cout << "Client: Strategy is set to reverse sorting.\n";
	context.setStrategy(new ConcreteStrategyB);
	context.doSomeBusinessLogic();
}
