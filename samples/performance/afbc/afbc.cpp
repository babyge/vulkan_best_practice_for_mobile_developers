/* Copyright (c) 2019, Arm Limited and Contributors
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge,
 * to any person obtaining a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "afbc.h"

#include "common/vk_common.h"
#include "gltf_loader.h"
#include "gui.h"
#include "platform/filesystem.h"
#include "platform/platform.h"
#include "rendering/subpasses/forward_subpass.h"
#include "stats.h"

#if defined(VK_USE_PLATFORM_ANDROID_KHR)
#	include "platform/android/android_platform.h"
#endif

AFBCSample::AFBCSample()
{
	auto &config = get_configuration();

	config.insert<vkb::BoolSetting>(0, afbc_enabled, false);
	config.insert<vkb::BoolSetting>(1, afbc_enabled, true);
}

bool AFBCSample::prepare(vkb::Platform &platform)
{
	if (!VulkanSample::prepare(platform))
	{
		return false;
	}

	// We want AFBC disabled at start-up
	afbc_enabled = false;
	recreate_swapchain();

	load_scene("scenes/sponza/Sponza01.gltf");

	auto &camera_node = vkb::add_free_camera(*scene, "main_camera", get_render_context().get_surface_extent());
	camera            = &camera_node.get_component<vkb::sg::Camera>();

	vkb::ShaderSource vert_shader("base.vert");
	vkb::ShaderSource frag_shader("base.frag");
	auto              scene_subpass = std::make_unique<vkb::ForwardSubpass>(get_render_context(), std::move(vert_shader), std::move(frag_shader), *scene, *camera);

	auto render_pipeline = vkb::RenderPipeline();
	render_pipeline.add_subpass(std::move(scene_subpass));

	set_render_pipeline(std::move(render_pipeline));

	stats = std::make_unique<vkb::Stats>(std::set<vkb::StatIndex>{vkb::StatIndex::l2_ext_write_bytes});
	gui   = std::make_unique<vkb::Gui>(*this, platform.get_window().get_dpi_factor());

	return true;
}

void AFBCSample::update(float delta_time)
{
	if (afbc_enabled != afbc_enabled_last_value)
	{
		recreate_swapchain();

		afbc_enabled_last_value = afbc_enabled;
	}

	VulkanSample::update(delta_time);
}

void AFBCSample::recreate_swapchain()
{
	std::set<VkImageUsageFlagBits> image_usage_flags = {VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT};

	if (!afbc_enabled)
	{
		// To force-disable AFBC, set an invalid image usage flag
		image_usage_flags.insert(VK_IMAGE_USAGE_STORAGE_BIT);
	}

	get_device().wait_idle();

	get_render_context().update_swapchain(image_usage_flags);
}

void AFBCSample::draw_gui()
{
	gui->show_options_window(
	    /* body = */ [this]() {
		    ImGui::Checkbox("AFBC", &afbc_enabled);
	    },
	    /* lines = */ 1);
}

std::unique_ptr<vkb::VulkanSample> create_afbc()
{
	return std::make_unique<AFBCSample>();
}
