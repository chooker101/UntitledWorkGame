#pragma once

#ifdef _DEBUG
#define HALCYONIC_DEBUG(statement, message)			\
	if(!statement)									\
	{												\
		if(_WIN32)									\
		{											\
			std::string output = message;			\
			output += "/n";							\
			OutputDebugStringA(output.c_str());		\
		}											\
		assert(statement);							\
	}

#define HALCYONIC_VK_CHECK(result, message)																						\
{																																\
	if (result != VK_SUCCESS)																									\
	{																															\
		std::string output = message;																							\
																																\
		switch (result)																											\
		{																														\
			case VK_NOT_READY						 : output += " VkReturn: VK_NOT_READY						"; break;		\
			case VK_TIMEOUT							 : output += " VkReturn: VK_TIMEOUT							"; break;		\
			case VK_EVENT_SET						 : output += " VkReturn: VK_EVENT_SET						"; break;		\
			case VK_EVENT_RESET						 : output += " VkReturn: VK_EVENT_RESET						"; break;		\
			case VK_INCOMPLETE						 : output += " VkReturn: VK_INCOMPLETE						"; break;		\
			case VK_ERROR_OUT_OF_HOST_MEMORY		 : output += " VkReturn: VK_ERROR_OUT_OF_HOST_MEMORY		"; break;		\
			case VK_ERROR_OUT_OF_DEVICE_MEMORY		 : output += " VkReturn: VK_ERROR_OUT_OF_DEVICE_MEMORY		"; break;		\
			case VK_ERROR_INITIALIZATION_FAILED		 : output += " VkReturn: VK_ERROR_INITIALIZATION_FAILED		"; break;		\
			case VK_ERROR_DEVICE_LOST				 : output += " VkReturn: VK_ERROR_DEVICE_LOST				"; break;		\
			case VK_ERROR_MEMORY_MAP_FAILED			 : output += " VkReturn: VK_ERROR_MEMORY_MAP_FAILED			"; break;		\
			case VK_ERROR_LAYER_NOT_PRESENT			 : output += " VkReturn: VK_ERROR_LAYER_NOT_PRESENT			"; break;		\
			case VK_ERROR_EXTENSION_NOT_PRESENT		 : output += " VkReturn: VK_ERROR_EXTENSION_NOT_PRESENT		"; break;		\
			case VK_ERROR_FEATURE_NOT_PRESENT		 : output += " VkReturn: VK_ERROR_FEATURE_NOT_PRESENT		"; break;		\
			case VK_ERROR_INCOMPATIBLE_DRIVER		 : output += " VkReturn: VK_ERROR_INCOMPATIBLE_DRIVER		"; break;		\
			case VK_ERROR_TOO_MANY_OBJECTS 			 : output += " VkReturn: VK_ERROR_TOO_MANY_OBJECTS 			"; break;		\
			case VK_ERROR_FORMAT_NOT_SUPPORTED		 : output += " VkReturn: VK_ERROR_FORMAT_NOT_SUPPORTED		"; break;		\
			case VK_ERROR_FRAGMENTED_POOL			 : output += " VkReturn: VK_ERROR_FRAGMENTED_POOL			"; break;		\
			case VK_ERROR_OUT_OF_POOL_MEMORY		 : output += " VkReturn: VK_ERROR_OUT_OF_POOL_MEMORY		"; break;		\
			case VK_ERROR_INVALID_EXTERNAL_HANDLE	 : output += " VkReturn: VK_ERROR_INVALID_EXTERNAL_HANDLE	"; break;		\
			case VK_ERROR_SURFACE_LOST_KHR			 : output += " VkReturn: VK_ERROR_SURFACE_LOST_KHR			"; break;		\
			case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR	 : output += " VkReturn: VK_ERROR_NATIVE_WINDOW_IN_USE_KHR	"; break;		\
			case VK_SUBOPTIMAL_KHR					 : output += " VkReturn: VK_SUBOPTIMAL_KHR					"; break;		\
			case VK_ERROR_OUT_OF_DATE_KHR			 : output += " VkReturn: VK_ERROR_OUT_OF_DATE_KHR			"; break;		\
			case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR	 : output += " VkReturn: VK_ERROR_INCOMPATIBLE_DISPLAY_KHR	"; break;		\
			case VK_ERROR_VALIDATION_FAILED_EXT		 : output += " VkReturn: VK_ERROR_VALIDATION_FAILED_EXT		"; break;		\
			case VK_ERROR_INVALID_SHADER_NV			 : output += " VkReturn: VK_ERROR_INVALID_SHADER_NV			"; break;		\
			case VK_ERROR_NOT_PERMITTED_EXT			 : output += " VkReturn: VK_ERROR_NOT_PERMITTED_EXT			"; break;		\
			default									 : output += " VkReturn: INVALID RETURN TYPE				"; break;		\
		}																														\
																																\
		if (_WIN32)																												\
		{																														\
			output += "/n";																										\
			OutputDebugStringA(output.c_str());																					\
		}																														\
	}																															\
}																																
#else
#define HALCYONIC_DEBUG(statement, message)
#define HALCYONIC_VK_CHECK(result, message)
#endif // DEBUG
