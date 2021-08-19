#pragma once
#include "Engine/General/SystemInclude.h"
#include "Engine/General/EntryPoint.h"
#include "Engine/General/Event.h"
#include "Engine/General/Logger.h"
#include "Engine/General/Window.h"

#include "Engine/Utilities/Exception.h"
#include "Engine/Utilities/String.h"
#include "Engine/Utilities/Timer.h"
#include "Engine/Utilities/Vector.h"
#include "Engine/Utilities/Range.h"

#include "Engine/Graphics/Instance.h"
#include "Engine/Graphics/DebugMessenger.h"
#include "Engine/Graphics/Device.h"
#include "Engine/Graphics/VulkanPointer.h"
#include "Engine/Graphics/Swapchain.h"
#include "Engine/Graphics/Shader.h"
#include "Engine/Graphics/Pipeline.h"
#include "Engine/Graphics/FrameBuffer.h"
#include "Engine/Graphics/Command.h"
#include "Engine/Graphics/Sync.h"
#include "Engine/Graphics/Frame.h"
#include "Engine/Graphics/Vertex.h"
#include "Engine/Graphics/Buffer.h"
#include "Engine/Graphics/VertexBuffer.h"
#include "Engine/Graphics/IndexBuffer.h"
#include "Engine/Graphics/ResourceAllocator.h"

#include "Engine/Resources/BakedResource.h"

using namespace std::chrono_literals;