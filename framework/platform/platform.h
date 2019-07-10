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

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "application.h"
#include "common/vk_common.h"
#include "platform/argument_parser.h"
#include "utils.h"

namespace vkb
{
class Application;

enum class ExitCode
{
	Success,
	Fatal
};

class Platform
{
  public:
	Platform();

	virtual ~Platform() = default;

	/**
	 * @brief Sets up windowing system and logging
	 */
	virtual bool initialize(std::unique_ptr<Application> &&app);

	virtual VkSurfaceKHR create_surface(VkInstance instance) = 0;

	virtual void main_loop() = 0;

	virtual void terminate(ExitCode code);

	virtual void close() const = 0;

	/**
	 * @return The dot-per-inch scale factor
	 */
	virtual float get_dpi_factor() const;

	const ArgumentParser &get_arguments();

	Application &get_app() const;

	/**
	 * @brief Generates an argument map from a string of input arguments
	 */
	void parse_arguments(const std::string &argument_string);

	std::string &get_log_output_path();

  protected:
	void prepare_logger(std::vector<spdlog::sink_ptr> sinks = {});

	std::unique_ptr<Application> active_app;

	ArgumentParser arguments{""};

  private:
	std::string log_output;
};
}        // namespace vkb
