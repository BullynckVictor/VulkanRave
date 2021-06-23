#include "Engine/Engine.h"

void main()
{
	rv::Timer timer;
	rv::WindowManager manager;
	rv::Window& window = manager.Create("Vulkan Rave Application", 480, 320, true);
	rv::EventListener listener = window.Listen();
	rv::Instance instance;
	#ifdef RV_DEBUG
	rv::DebugMessenger messenger(instance);
	rv::EventListener errorListener = messenger.Listen();
	#endif
	rv::vk::Surface surface = window.Surface(instance);
	rv::Device device(instance, rv::GraphicsRequirements(surface.get()));
	rv::DeviceQueue graphicsQueue = device.Queue({ rv::QueueContainsFlag, VK_QUEUE_GRAPHICS_BIT });
	rv::DeviceQueue presentQueue = device.Queue({ rv::QueueCanPresent, surface.get() });
	rv::Shader frag = rv::Shader(device, "../Engine/Graphics/Shaders/bin/triangle.frag.spv");
	rv::Shader vert = rv::Shader(device, "../Engine/Graphics/Shaders/bin/triangle.vert.spv");
	rv::SwapChain swap(device, surface.get(), rv::DefaultSwap(), window.GetSize());

	rv::CommandPool pool(device, graphicsQueue);

	std::vector<rv::CommandBuffer> commandBuffers(swap.images.size());

	rv::PipelineLayout layout;
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
	rv::Pipeline pipeline(device, layout);

	std::vector<rv::FrameBuffer> frameBuffers(swap.images.size());
	for (auto i : rv::range(frameBuffers))
		frameBuffers[i] = rv::FrameBuffer(device, layout.pass, swap.views[i], { swap.extent.width, swap.extent.height });

	for (auto i : rv::range(commandBuffers))
	{
		commandBuffers[i] = rv::CommandBuffer(device, pool);
		commandBuffers[i].Begin();
		commandBuffers[i].BeginPass(layout.pass, frameBuffers[i], window.GetSize(), rv::FColors::LightGray);
		commandBuffers[i].BindPipeline(pipeline);
		commandBuffers[i].Draw(3);
		commandBuffers[i].EndPass();
		commandBuffers[i].End();
	}

	std::vector<rv::Frame> frames(2);
	size_t currentFrame = 0;
	for (auto& frame : frames)
		frame = rv::Frame(device);

	std::vector<VkFence> inFlight(swap.images.size(), VK_NULL_HANDLE);

	rv::uint nFrames = 0;
	rv::debug.Log(rv::str("init time: ", timer.Mark(), "s"));
	while (window.Open())
	{
		while (auto e = listener.Get())
			if (auto event = e->opt_cast<rv::WindowResizeEvent>())
				rv::debug.Log(rv::str("Window resize: (", event->size.width, ", ", event->size.height, ")"));
		if constexpr (rv::build.debug)
			while (auto e = errorListener.Get())
			{
				if (auto event = e->opt_cast<rv::VulkanDebugEvent>())
				{
					window.SetTitle(event->message);
					if (event->messageType == rv::RV_MT_ERROR || event->messageType == rv::RV_MT_WARNING)
						rv_throw(rv::str("Debug Callback: ", event->message));
				}
			}

		if (!window.Minimized())
		{
			/*
			rv::u32 index = swap.AcquireNextImage(device, imageAvailable, {});
			VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
			commandBuffers[index].Submit(graphicsQueue, &imageAvailable.semaphore, waitStages, 1, &renderFinished.semaphore, 1);
			swap.Present(presentQueue, index, &renderFinished.semaphore, 1);
			*/
			frames[currentFrame].Begin(device, swap, inFlight.data());
			frames[currentFrame].Submit(device, graphicsQueue, commandBuffers[frames[currentFrame].image]);
			frames[currentFrame].Present(swap, presentQueue);
		}

		currentFrame = (currentFrame + 1) % frames.size();
		nFrames++;
	}
	rv::debug.Log(rv::str((float)nFrames / timer.Peek(), " fps"));
}