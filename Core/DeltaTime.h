#pragma once
class DeltaTime
{
public:
	DeltaTime();
	~DeltaTime();

	void CalculateDeltaTime();
	double GetDeltaTime();

private:
	double m_LastFrameTime;
	double m_DeltaTime;
};