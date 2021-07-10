#include "Engine/Engine.h"
#include "Engine/Resources/triangle.frag.spv.h"
#include "Engine/Resources/triangle.vert.spv.h"

class TestApp
{
public:
	TestApp()
		:
		window("Test App", 480, 320, true),
		listener(window.Listen()),
#ifdef RV_DEBUG
		messenger(instance),
		errorListener(messenger.Listen()),
#endif
		surface(window.Surface(instance)),
		device(instance, rv::GraphicsRequirements(surface.get())),
		graphicsQueue(device.Queue({ rv::QueueContainsFlag, VK_QUEUE_GRAPHICS_BIT })),
		presentQueue(device.Queue({ rv::QueueCanPresent, surface.get() })),
		frag(device, rv::Resources::triangle_frag_spv, rv::RV_ST_FRAGMENT),
		vert(device, rv::Resources::triangle_vert_spv, rv::RV_ST_VERTEX),
		pool(device, graphicsQueue),
		frames(2),
		vertices(device, {
			{{ 0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{ 0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}}
		})
	{
		for (auto& frame : frames)
			frame = rv::Frame(device);
		RecreateSwap();
	}

	void Go()
	{
		std::vector<VkFence> inFlight(swap.images.size(), VK_NULL_HANDLE);
		rv::uint nFrames = 0;
		size_t currentFrame = 0;

		float t = 0.0f;
		std::vector<float> fpses;

		rv::debug.Log(rv::str("init time: ", timer.Mark(), "s"));
		rv::Timer s{};
		while (window.HandleMessages())
		{
			t += s.Peek();
			
			while (auto e = listener.Get())
				if (auto event = e->opt_cast<rv::WindowResizeEvent>())
					rv::debug.Log(rv::str("Window resize: (", event->size.width, ", ", event->size.height, ")"));
#		ifdef RV_DEBUG
			while (auto e = errorListener.Get())
			{
				if (auto event = e->opt_cast<rv::VulkanDebugEvent>())
				{
					window.SetTitle(event->message);
					if (event->messageType == rv::RV_MT_ERROR || event->messageType == rv::RV_MT_WARNING)
						rv_throw(rv::str("Debug Callback: ", event->message));
				}
			}
#		endif
			
			if (!window.Minimized())
			{
				if (frames[currentFrame].Begin(device, swap, inFlight.data()) && !window.Minimized())
				{
					rv::debug.Log("Resize! (Begin)");
					RecreateSwap();
					continue;
				}
				frames[currentFrame].Submit(device, graphicsQueue, commandBuffers[frames[currentFrame].image]);
				if (frames[currentFrame].Present(swap, presentQueue) && !window.Minimized())
				{
					rv::debug.Log("Resize! (Present)");
					RecreateSwap();
					continue;
				}
			}

			currentFrame = (currentFrame + 1) % frames.size();
			nFrames++;
			window.SetTitle(rv::str(int(1.0f / s.Peek())));
			fpses.push_back(1.0f / s.Peek());
			s.Reset();
		}
		rv::debug.Log(rv::str((float)nFrames / timer.Peek(), " fps"));
	}
	~TestApp()
	{
		vkDeviceWaitIdle(device.device);
	}

private:
	void RecreateSwap()
	{
		vkQueueWaitIdle(graphicsQueue.queue);
		vkQueueWaitIdle(presentQueue.queue);

		frameBuffers.clear();
		pipeline.Release();
		layout.Release();
		layout.Clear();
		swap.Release();

		swap = rv::SwapChain(device, surface.get(), rv::DefaultSwap(true), window.GetSize());
		if (commandBuffers.size() != swap.images.size())
		{
			commandBuffers.clear();
			commandBuffers.resize(swap.images.size());
		}
		else
		{
			for (auto& cmd : commandBuffers)
				cmd.Free(device, pool);
		}
		for (auto& cmd : commandBuffers)
			cmd = rv::CommandBuffer(device, pool);

		rv::RenderPassDescriptor pass;
		pass.CreateSubpass();
		pass.subpasses[0].AddColor(rv::attachments::Color(swap.format.format));
		pass.dependencies.push_back(rv::dependencies::Color(0));
		layout.pass = rv::RenderPass(device, pass);
		layout.AddShader(frag);
		layout.AddShader(vert);
		layout.SetBlending(false);
		layout.SetCulling(true);
		layout.SetSize(window.GetSize());
		layout.SetVertexType<rv::ColorVertex2>();
		layout.Finalize(device);
		pipeline = rv::Pipeline(device, layout);

		frameBuffers.resize(swap.images.size());
		for (auto i : rv::range(frameBuffers))
			frameBuffers[i] = rv::FrameBuffer(device, layout.pass, swap.views[i], { swap.extent.width, swap.extent.height });

		for (auto i : rv::range(commandBuffers))
		{
			commandBuffers[i].Begin();
			commandBuffers[i].BeginPass(layout.pass, frameBuffers[i], window.GetSize(), rv::FColors::LightGray);
			commandBuffers[i].BindPipeline(pipeline);
			commandBuffers[i].BindVertexBuffer(vertices);
			commandBuffers[i].Draw(vertices.size());
			commandBuffers[i].EndPass();
			commandBuffers[i].End();
		}
	}

	rv::Timer timer;
	rv::Window window;
	rv::EventListener listener;
	rv::Instance instance;
#ifdef RV_DEBUG
	rv::DebugMessenger messenger;
	rv::EventListener errorListener;
#endif
	rv::vk::Surface surface;
	rv::Device device;
	rv::DeviceQueue graphicsQueue;
	rv::DeviceQueue presentQueue;
	rv::Shader frag;
	rv::Shader vert;
	rv::SwapChain swap;
	rv::CommandPool pool;
	std::vector<rv::CommandBuffer> commandBuffers;
	rv::PipelineLayout layout;
	rv::Pipeline pipeline;
	std::vector<rv::FrameBuffer> frameBuffers;
	std::vector<rv::Frame> frames;
	std::vector<VkFence> inFlight;
	rv::VertexBufferT<rv::ColorVertex2> vertices;
};

void main()
{
	TestApp{}.Go();
}