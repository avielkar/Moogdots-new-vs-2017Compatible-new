/////////////////////////////////////////////////////////////////////////////////////////////////
//	Inline function definitions files.
//
//	@Author:	Christopher Broussard
//	@Date:		5/15/2003
/////////////////////////////////////////////////////////////////////////////////////////////////

inline double MoogCom::ThreadGetReceiveTime() const
{
	return m_receiveTime / m_clockFrequency * 1000.0;
}


inline double MoogCom::ThreadGetSendTime() const
{
	return m_sendTime / m_clockFrequency * 1000.0;
}


inline double MoogCom::ThreadGetReturnedLateral() const
{
	return m_dofValues[DOF_LATERAL_INDEX];
}


inline double MoogCom::ThreadGetReturnedHeave() const
{
	return m_dofValues[DOF_HEAVE_INDEX];
}


inline double MoogCom::ThreadGetReturnedSurge() const
{
	return m_dofValues[DOF_SURGE_INDEX];
}


inline double MoogCom::ThreadGetReturnedRoll() const
{
	return m_dofValues[DOF_ROLL_INDEX]/PI*180.0;
}


inline double MoogCom::ThreadGetReturnedPitch() const
{
	return m_dofValues[DOF_PITCH_INDEX]/PI*180.0;
}


inline double MoogCom::ThreadGetReturnedYaw() const
{
	return m_dofValues[DOF_YAW_INDEX]/PI*180.0;
}