#include "Engine/Engine.h"

class TestApp
{
public:
	TestApp()
		:
		window("Test App", 480, 320, true),
		listener(window.Listen()),
		messenger(instance),
		errorListener(messenger.Listen()),
		surface(window.Surface(instance)),
		device(instance, rv::GraphicsRequirements(surface.get())),
		graphicsQueue(device.Queue({ rv::QueueContainsFlag, VK_QUEUE_GRAPHICS_BIT })),
		presentQueue(device.Queue({ rv::QueueCanPresent, surface.get() })),
		frag(device, "../Engine/Graphics/Shaders/bin/triangle.frag.spv"),
		vert(device, "../Engine/Graphics/Shaders/bin/triangle.vert.spv"),
		pool(device, graphicsQueue),
		frames(2)
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

		rv::debug.Log(rv::str("init time: ", timer.Mark(), "s"));
		while (window.HandleMessages())
		{
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
		}
		rv::debug.Log(rv::str((float)nFrames / timer.Peek(), " fps"));
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
			commandBuffers[i].Draw(3);
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
};

void main()
{
	TestApp{}.Go();
}