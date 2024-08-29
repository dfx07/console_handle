////////////////////////////////////////////////////////////////////////////////////
/*!*********************************************************************************
*         Copyright (C) 2023-2024 thuong.nv <thuong.nv.mta@gmail.com>               
*                   MIT software Licencs, see the accompanying                      
************************************************************************************
* @brief : Timer, Stopwatch, FPSCounter
* @file  : xtime.h
* @create: Aug 28, 2024
* @note  : For conditions of distribution and use, see copyright notice in readme.txt
***********************************************************************************/
#ifndef XTIMER_H
#define XTIMER_H

#include <chrono>
#include <ctime>
#include <string>

////////////////////////////////////////////////////////////////////////////////////
/**********************************************************************************/
// __Timer class interface
class __Timer
{
protected:
	typedef std::chrono::steady_clock::time_point	  time_pointer;
	typedef std::chrono::duration<double>			  tduration;

protected:

	/*******************************************************************************
	*! @brief  : get current time
	*! @return : time_pointer : time point
	*! @author : thuong.nv - [Date] : 2023.02.18
	*******************************************************************************/
	static time_pointer now()
	{
		return std::chrono::steady_clock::now();
	}

	/*******************************************************************************
	*! @brief  : get current time follow the specified format
	*! @return : string
	*! @param  : [in] format : "%Y-%m-%d %X"
	*! @author : thuong.nv - [Date] : 2023.02.18
	*! @note   : https://www.programiz.com/python-programming/datetime/strftime
	*******************************************************************************/
	static std::string now_to_string(const char* format = "%Y-%m-%d %X")
	{
		auto now = std::chrono::system_clock::now();
		std::time_t end_time = std::chrono::system_clock::to_time_t(now);

		struct tm  tstruct;
		auto err = localtime_s(&tstruct, &end_time);
		char buffer[128];
		memset(buffer, 0, sizeof(buffer));

		strftime(buffer, sizeof(buffer), format, &tstruct);

		return std::string(buffer);
	}
};


////////////////////////////////////////////////////////////////////////////////////
/**********************************************************************************/
// Timer class
class Timer : public __Timer
{
public:
	Timer()
	{
		reset();
	}

public:
	/*******************************************************************************
	*! @brief  : get time from previous retrieval
	*! @return : double (seconds)
	*! @author : thuong.nv - [Date] : 2023.02.18
	*******************************************************************************/
	void reset()
	{
		m_tstart = __Timer::now();
	}

	/*******************************************************************************
	*! @brief  : get time from previous retrieval
	*! @return : double (seconds)
	*! @author : thuong.nv - [Date] : 2023.02.18
	*******************************************************************************/
	double elapsed_to_seconds() const noexcept
	{
		time_pointer tend = __Timer::now();
		tduration elapsed = tend - m_tstart;
		return elapsed.count();
	}

	/******************************************************************************
	*! @brief  : get time from previous retrieval
	*! @return : double (millisecond)
	*! @author : thuong.nv - [Date] : 2023.02.18
	*******************************************************************************/
	double elapsed_to_mili() const noexcept
	{
		auto elp = elapsed_to_seconds();
		return elp * 1000.0;
	}

private:
	time_pointer		m_tstart;
};


////////////////////////////////////////////////////////////////////////////////////
/**********************************************************************************/
// StopWatch class
class StopWatch : public __Timer
{
public:
	StopWatch() : m_dur(0.0),
		m_bpause(true),
		m_delapsed(0.0)
	{

	}

public:
	/*******************************************************************************
	*! @brief  : begin a measurement session
	*! @return : void
	*! @author : thuong.nv - [Date] : 2023.02.18
	*******************************************************************************/
	void start()
	{
		m_tstart = __Timer::now();
		m_bpause = false;
	}

	/*******************************************************************************
	*! @brief  : reset stopwatch
	*! @return : void
	*! @author : thuong.nv - [Date] : 2023.02.18
	*******************************************************************************/
	void reset()
	{
		m_bpause = true;
		m_delapsed = m_dur = 0.0;
	}

	/*******************************************************************************
	*! @brief  : stop stopwatch, can continue
	*! @return : void
	*! @author : thuong.nv - [Date] : 2023.02.18
	*******************************************************************************/
	void pause()
	{
		if (m_bpause)
			return;

		tduration elapsed = __Timer::now() - m_tstart;
		m_delapsed = elapsed.count();

		m_dur += m_delapsed;

		m_bpause = true;
	}

	/*******************************************************************************
	*! @brief  : elapsed time since the beginning but not stop
	*! @return : double (millisecond)
	*! @author : thuong.nv - [Date] : 2023.02.18
	*******************************************************************************/
	double split() const noexcept
	{
		if (m_bpause)
			return 0.0;

		tduration elapsed = __Timer::now() - m_tstart;
		return elapsed.count() + m_dur;
	}

	/*******************************************************************************
	*! @brief  : check pause
	*! @return : bool (true/false)
	*! @author : thuong.nv - [Date] : 2023.02.18
	*******************************************************************************/
	bool is_pause() const noexcept
	{
		return m_bpause;
	}

	/*******************************************************************************
	*! @brief  : elapsed time without the last count
	*! @return : double (millisecond)
	*! @author : thuong.nv - [Date] : 2023.02.18
	*******************************************************************************/
	double elapsed_to_seconds() const noexcept
	{
		return m_dur * 1000.0;
	}

	double elapsed_to_mili() const noexcept
	{
		return m_dur;
	}

private:
	time_pointer	m_tstart;
	double			m_delapsed;

	double			m_dur; // all time
	bool			m_bpause;
};


////////////////////////////////////////////////////////////////////////////////////
/**********************************************************************************/
// StopWatch class

class FPSCounter : public __Timer
{
public:
	FPSCounter() : m_fps(0), m_elapsed(0.0),
		m_reset(0.0), m_frames(0)
	{

	}

	/*******************************************************************************
	*! @brief  : start counter frames per second
	*! @return : void
	*! @author : thuong.nv - [Date] : 2023.02.18
	*******************************************************************************/
	void start()
	{
		m_last_frame = __Timer::now();
		m_frames = 0;
		m_reset = 0.0;
	}

	/*******************************************************************************
	*! @brief  : Update time passed since last frame
	*! @return : void
	*! @author : thuong.nv - [Date] : 2023.02.18
	*******************************************************************************/
	void update()
	{
		time_pointer cur_frame = __Timer::now();
		m_elapsed = tduration(cur_frame - m_last_frame).count();
		m_reset += m_elapsed;

		m_frames++;

		if (m_reset >= 1.0) // 1 second
		{
			m_fps = (unsigned int)(m_frames / m_reset);
			m_frames = 0;
			m_reset = 0.0;
		}

		m_last_frame = cur_frame;
	}

public:

	/*******************************************************************************
	*! @brief  : get frames per second
	*! @return : int
	*! @author : thuong.nv - [Date] : 2023.02.18
	*******************************************************************************/
	unsigned int fps() const noexcept
	{
		return m_fps;
	}

	/*******************************************************************************
	*! @brief  : last frame time
	*! @return : double //miliseconds
	*! @author : thuong.nv - [Date] : 2023.02.18
	*******************************************************************************/
	double frametime() const noexcept
	{
		return m_elapsed;
	}

private:
	unsigned int	m_fps;
	unsigned int	m_frames;
	double			m_elapsed;

	time_pointer	m_last_frame;

	double			m_reset;
};

#endif // ! XTIMER_H