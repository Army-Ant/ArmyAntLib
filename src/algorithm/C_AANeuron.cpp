/*  
 * Copyright (c) 2015 ArmyAnt
 * 版权所有 (c) 2015 ArmyAnt
 *
 * Licensed under the BSD License, Version 2.0 (the License); 
 * 本软件使用BSD协议保护, 协议版本:2.0
 * you may not use this file except in compliance with the License. 
 * 使用本开源代码文件的内容, 视为同意协议
 * You can read the license content in the file "LICENSE" at the root of this project
 * 您可以在本项目的根目录找到名为"LICENSE"的文件, 来阅读协议内容
 * You may also obtain a copy of the License at 
 * 您也可以在此处获得协议的副本:
 * 
 *     http://opensource.org/licenses/BSD-3-Clause
 * 
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an AS IS BASIS, 
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
 * 除非法律要求或者版权所有者书面同意,本软件在本协议基础上的发布没有任何形式的条件和担保,无论明示的或默许的.
 * See the License for the specific language governing permissions and limitations under the License. 
 * 请在特定限制或语言管理权限下阅读协议
 * This file is the internal source file of this project, is not contained by the closed source release part of this software
 * 本文件为内部源码文件, 不会包含在闭源发布的本软件中
 */

#include "../base/base.hpp"
#include "../../inc/AANeuron.hpp"
#include "../../inc/C_AANeuron.h"

#include <map>
#include <cmath>

typedef ArmyAnt::NeuronAlgorithm<double> C_AA_NeuronAlgorithm;

ARMYANT_CLIB_API double AA_Math_Liner(double input, double slope, double bias)
{
	return slope*input + bias;
}

ARMYANT_CLIB_API double AA_Math_Ramp(double input, double width, double height)
{
	if(input > width)
		return height;
	if(input < -width)
		return -height;
	return input*height / width;
}

ARMYANT_CLIB_API double AA_Math_Threshold(double input, double amplitude, double threshold)
{
	return input > threshold ? amplitude : 0.0;
}

ARMYANT_CLIB_API double AA_Math_Sigmoid(double input, double coef)
{
	return 1 / (1 + pow(ArmyAnt::Constant::c_natrualBase, -coef*input));
}

ARMYANT_CLIB_API double AA_Math_DoubleSigmoid(double input, double coef)
{
	return 2 / (1 + pow(ArmyAnt::Constant::c_natrualBase, -coef*input)) - 1;
}

// TODO : Bad use of global variable, must change it to "getInstance"
static std::map<AA_CNeuronAlgorithm, ArmyAnt::NeuronAlgorithm<double>*> manager;

ARMYANT_CLIB_API AA_CNeuronAlgorithm AA_NeuronAlgorithm_Create(AA_Neuron_ActiveFunc activationFunc, double threshold)
{
	uint32 len = uint32(manager.size());
	//获取当前未使用的最小句柄号
	for(uint32 n = 0; n < len; n++)
	{
		//设置句柄，创建内部实例，并关联到外部实例
		if(manager.find(n) == manager.end())
		{
			manager.insert(std::pair<uint32, ArmyAnt::NeuronAlgorithm<double>*>(n, new ArmyAnt::NeuronAlgorithm<double>(activationFunc, threshold)));
			return n;
		}
	}
	//没有未使用的中间句柄号，则在结尾添加新最大句柄号，并创建内部实例、关联到外部实例
	manager.insert(std::pair<uint32, ArmyAnt::NeuronAlgorithm<double>*>(len, new ArmyAnt::NeuronAlgorithm<double>(activationFunc, threshold)));
	return len;
}

ARMYANT_CLIB_API AA_CNeuronAlgorithm AA_NeuronAlgorithm_Clone(AA_CNeuronAlgorithm value)
{
	AAAssert(manager.find(value)!=manager.end(), 0U);
	uint32 len = uint32(manager.size());
	//获取当前未使用的最小句柄号
	for(uint32 n = 0; n < len; n++)
	{
		//设置句柄，创建内部实例，并关联到外部实例
		if(manager.find(n) == manager.end())
		{
			manager.insert(std::pair<uint32, ArmyAnt::NeuronAlgorithm<double>*>(n, new ArmyAnt::NeuronAlgorithm<double>(*manager.find(value)->second)));
			return n;
		}
	}
	//没有未使用的中间句柄号，则在结尾添加新最大句柄号，并创建内部实例、关联到外部实例
	manager.insert(std::pair<uint32, ArmyAnt::NeuronAlgorithm<double>*>(len, new ArmyAnt::NeuronAlgorithm<double>(*manager.find(value)->second)));
	return len;
}

ARMYANT_CLIB_API void AA_NeuronAlgorithm_Release(AA_CNeuronAlgorithm value)
{
	auto ret = manager.find(value);
	//销毁内部实例，解除关联
	if(ret != manager.end())
	{
		AA_SAFE_DEL(ret->second);
		manager.erase(ret);
	}
}

ARMYANT_CLIB_API BOOL AA_NeuronAlgorithm_JoinActive(AA_CNeuronAlgorithm value, double input, double weight)
{
	return manager.find(value)->second->JoinActive(input, weight) ? TRUE : FALSE;
}

ARMYANT_CLIB_API BOOL AA_NeuronAlgorithm_LeaveActive(AA_CNeuronAlgorithm value, double input, double weight)
{
	return manager.find(value)->second->LeaveActive(input, weight) ? TRUE : FALSE;
}

ARMYANT_CLIB_API BOOL AA_NeuronAlgorithm_ClearActive(AA_CNeuronAlgorithm value)
{
	return manager.find(value)->second->ClearActive() ? TRUE : FALSE;
}

ARMYANT_CLIB_API double AA_NeuronAlgorithm_GetOutput(AA_CNeuronAlgorithm value)
{
	return manager.find(value)->second->GetOutput();
}

ARMYANT_CLIB_API double AA_NeuronAlgorithm_GetAllActive(AA_CNeuronAlgorithm value)
{
	return manager.find(value)->second->GetAllActive();
}

ARMYANT_CLIB_API BOOL AA_NeuronAlgorithm_SetActivationFunction(AA_CNeuronAlgorithm value, AA_Neuron_ActiveFunc func)
{
	return manager.find(value)->second->SetActivationFunction(func) ? TRUE : FALSE;
}

ARMYANT_CLIB_API double AA_NeuronAlgorithm_GetThreshold(AA_CNeuronAlgorithm value)
{
	return manager.find(value)->second->GetThreshold();
}

ARMYANT_CLIB_API BOOL AA_NeuronAlgorithm_SetThreshold(AA_CNeuronAlgorithm value, double threshold)
{
	return manager.find(value)->second->SetThreshold(threshold) ? TRUE : FALSE;
}

ARMYANTLIB_API ArmyAnt::NeuronAlgorithm<double>* ArmyAnt::NeuronAlgorithm_GetCppObject(AA_CNeuronAlgorithm value)
{
	auto ret = manager.find(value);
	return (ret != manager.end()) ? ret->second : nullptr;
}
