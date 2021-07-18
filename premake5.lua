workspace "Vulkan-Engine"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
vulkanDir = "C:/VulkanSDK/1.2.176.1"

include "Vulkan-Engine/ext/glfw"

project "Vulkan-Engine"
    location "Vulkan-Engine"
    kind "ConsoleApp"
    language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}

    libdirs 
	{
		"%{vulkanDir}/Lib",
	}

    links
    {
        "GLFW",
        "vulkan-1.lib"
    }

    includedirs
	{
        "%{prj.name}/ext/glm",
        "%{prj.name}/ext/glfw/glfw/include",
        "%{vulkanDir}/include"
    }

    filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "10.0.18362.0"

        buildoptions "/MDd"
    
    filter "configurations:Debug"
		defines "VKE_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "VKE_RELEASE"
		optimize "On"