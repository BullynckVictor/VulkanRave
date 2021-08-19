
class Task
{
public:
	template<typename T>
	void NewTask(const T& task = {})
	{
		tasks.emplace_back(new T(task));
	}

private:
	std::vector<std::unique_pt<Task>> tasks;
};

struct RenderingTask : public Task
{
	bool Run() override
	{

	}
	Application& app;
};

struct UpdateTask : public Task
{
	bool Run() override
	{

	}
	Application& app;
};

void Go()
{
	threads.NewTask(RenderingTask());
	threads.NewTask(UpdateTask());
}