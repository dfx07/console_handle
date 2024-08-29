////////////////////////////////////////////////////////////////////////////////////
/*!*********************************************************************************
*         Copyright (C) 2023-2024 thuong.nv <thuong.nv.mta@gmail.com>               
*                   MIT software Licencs, see the accompanying                      
************************************************************************************
* @brief : Common context define
* @file  : xctx.h
* @create: Aug 14, 2024
* @note  : For conditions of distribution and use, see copyright notice in readme.txt
************************************************************************************/
#ifndef XCTX_H
#define XCTX_H

#include <memory>

#ifndef interface
#define interface struct
#endif

interface DeviceContext;
typedef std::shared_ptr<DeviceContext> DeviceContextPtr;

/////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************/
// IContext class

interface IContext
{
	virtual bool CreateContext(void* hHandle) = 0;
	virtual void DeleteContext() noexcept = 0;
	virtual bool MakeCurrentContext() const noexcept = 0;
	virtual void SwapBuffer() const noexcept = 0;
};

/////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************/
// DeviceContextConfig class

class DeviceContextConfig
{
public:
	enum Flags {
		USE_ANTIALIAS  = 0x00001,
		USE_OPENGL_EXT = 0x00002,
	};

public:
	void SetAntiliasingLevel(int nLevel) noexcept {
		m_nFlag |= USE_ANTIALIAS;
		m_nAntialiasingLevel = nLevel;
	}

	void UseOpenGLExtension(bool bUse) noexcept {
		m_nFlag |= USE_OPENGL_EXT;
	}

	void ClearFlag(unsigned int _nFlag) {
		m_nFlag &= ~_nFlag;
	}

	bool ValidFlag(int _nFlag) const noexcept { return m_nFlag & _nFlag; }
	int  GetAntiliasingLevel() const noexcept { return m_nAntialiasingLevel; }

protected:
	int m_nFlag{ 0 };
	int m_nAntialiasingLevel{ 0 }; // 0~8
};

/////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************/
// DeviceContextConfig class

interface DeviceContext : public IContext
{
	virtual void* Render() const noexcept = 0;
	virtual bool  IsValid() const noexcept = 0;

public:
	const DeviceContextConfig* GetConfig() const noexcept
	{
		return &Config;
	}

	void SetConfig(DeviceContextConfig& _Config) noexcept
	{
		Config = _Config;
	}

protected:
	DeviceContextConfig Config;
};



#endif // !XCTX_H