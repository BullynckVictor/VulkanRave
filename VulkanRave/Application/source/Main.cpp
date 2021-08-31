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
		graphicsQueue(device.StoreQueue("graphics", { rv::QueueContainsFlag, VK_QUEUE_GRAPHICS_BIT })),
		presentQueue(device.StoreQueue("present", { rv::QueueCanPresent, surface.get() })),
		allocator(instance, device, graphicsQueue),
		frag(device, rv::Resources::triangle_frag_spv, rv::RV_ST_FRAGMENT),
		vert(device, rv::Resources::triangle_vert_spv, rv::RV_ST_VERTEX),
		pool(device, graphicsQueue),
		frames(2),
		camera(rv::Vector2{ 0 }, 0, 1, window.GetSize()),
		triangle(device, allocator, camera)
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
			while (auto e = listener.Get())
				if (auto event = e->opt_cast<rv::WindowResizeEvent>())
					rv::debug.Log(rv::str("Window resize: (", event->size.x, ", ", event->size.y, ")"));
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

			float a = sinf((t + 1.0f) / 2.0f);
			triangle.color.b = a / 3.0f + 2.0f / 3.0f;
			triangle.color.g = a / 3.0f + 2.0f / 3.0f * 4;
			triangle.color.r = 0.0f;

			triangle.colorUniform.Copy(device, &triangle.color, sizeof(rv::FColor));
			triangle.transformUniform.Copy(device, &triangle.transform.modelview, sizeof(rv::Matrix4));
			
			if (!window.Minimized())
			{
				if (frames[currentFrame].Begin(device, swap, inFlight.data()) && !window.Minimized())
				{
					rv::debug.Log("Resize! (Begin)");
					RecreateSwap();
					continue;
				}
				frames[currentFrame].Submit(device, graphicsQueue, triangle.commandBuffers[frames[currentFrame].image]);
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
			t += s.Peek();
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
		triangle.pipeline.Release();
		triangle.layout.Release();
		triangle.layout.Clear();
		swap.Release();

		swap = rv::SwapChain(device, surface.get(), rv::DefaultSwap(true), window.GetSize());
		if (triangle.commandBuffers.size() != swap.images.size())
		{
			triangle.commandBuffers.clear();
			triangle.commandBuffers.resize(swap.images.size());
		}
		else
		{
			for (auto& cmd : triangle.commandBuffers)
				cmd.Free(device, pool);
		}
		for (auto& cmd : triangle.commandBuffers)
			cmd = rv::CommandBuffer(device, pool);

		rv::RenderPassDescriptor pass;
		pass.CreateSubpass();
		pass.subpasses[0].AddColor(rv::attachments::Clear(swap.format.format));
		pass.dependencies.push_back(rv::dependencies::Color(0));
		triangle.layout.Release();
		triangle.layout.pass = rv::RenderPass(device, pass);
		triangle.layout.AddShader(frag);
		triangle.layout.AddShader(vert);
		triangle.layout.SetBlending(false);
		triangle.layout.SetCulling(false);
		triangle.layout.SetSize(window.GetSize());
		triangle.layout.SetVertexType<rv::Vertex2>();
		triangle.layout.descriptorSet.AddUniformBuffer(rv::RV_ST_VERTEX);
		triangle.layout.descriptorSet.AddUniformBuffer(rv::RV_ST_FRAGMENT);
		triangle.layout.Finalize(device);
		triangle.pipeline = rv::Pipeline(device, triangle.layout);

		frameBuffers.resize(swap.images.size());
		for (auto i : rv::range(frameBuffers))
			frameBuffers[i] = rv::FrameBuffer(device, triangle.layout.pass, swap.views[i], { swap.extent.width, swap.extent.height });

		descPool = rv::DescriptorPool(device, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2);
		triangle.set = rv::DescriptorSet(device, descPool, triangle.layout.descriptorSet);
		triangle.set.WriteBuffer(device, triangle.transformUniform, sizeof(rv::Matrix4), 0);
		triangle.set.WriteBuffer(device, triangle.colorUniform, sizeof(rv::FColor), 1);

		for (auto i : rv::range(triangle.commandBuffers))
		{
			triangle.commandBuffers[i].Begin();
			triangle.commandBuffers[i].BeginPass(triangle.layout.pass, frameBuffers[i], window.GetSize(), rv::FColors::Black);
			triangle.commandBuffers[i].BindPipeline(triangle.pipeline);
			triangle.commandBuffers[i].BindVertexBuffer(triangle.vertices);
			triangle.commandBuffers[i].BindIndexBuffer(triangle.indices);
			triangle.commandBuffers[i].BindDescriptorSet(triangle.layout, triangle.set);
			triangle.commandBuffers[i].DrawIndexed((rv::u32)triangle.indices.size());
			triangle.commandBuffers[i].EndPass();
			triangle.commandBuffers[i].End();
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
	rv::ResourceAllocator allocator;
	rv::Shader frag;
	rv::Shader vert;
	rv::SwapChain swap;
	rv::CommandPool pool;	
	std::vector<rv::FrameBuffer> frameBuffers;
	std::vector<rv::Frame> frames;
	std::vector<VkFence> inFlight;
	rv::Camera2 camera{};
	rv::DescriptorPool descPool = {};
	

	struct Triangle
	{
		Triangle(rv::Device& device, rv::ResourceAllocator& allocator, const rv::Camera2& camera)
			:
			vertices(device, allocator, {
				{{ -0.5f, -0.5f }},
				{{  0.5f, -0.5f }},
				{{  0.5f,  0.5f }},
				{{ -0.5f,  0.5f }}
			}),
			indices(device, allocator, { 0, 1, 2, 2, 3, 0 }),
			transform(camera),
			colorUniform(device, allocator, color),
			transformUniform(device, allocator, transform.modelview),
			color(rv::FColors::White)
		{
			for (const auto& vert : vertices.vertices)
			{
				rv::Vector4 vec4 = rv::Vector4(vert.pos, rv::Vector2(0.0f, 1.0f));
				auto result = vec4 * camera.view;
				result.x += 1.0f;
			}
		}

		std::vector<rv::CommandBuffer> commandBuffers;
		rv::PipelineLayout layout;
		rv::Pipeline pipeline;
		rv::Transform2 transform;
		rv::FColor color;
		rv::UniformBuffer colorUniform;
		rv::UniformBuffer transformUniform;
		rv::VertexBufferT<rv::Vertex2> vertices;
		rv::IndexBuffer16 indices;
		rv::DescriptorSet set = {};
	} triangle;
};

void main()
{
	TestApp{}.Go();
}