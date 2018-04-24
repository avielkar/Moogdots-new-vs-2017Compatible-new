/////////////////////////////////////////////////////////////////////////////////////
//	GLWindow.inl
//
//	@Author:	Christopher Broussard
//	@Date:		5/15/2003
/////////////////////////////////////////////////////////////////////////////////////

inline World* GLPanel::GetWorld()
{
	return &m_world;
}

inline GLPanel * GLWindow::GetGLPanel() const
{
	return m_glpanel;
}


inline StarField * GLPanel::GetStarField()
{
	return &m_world.starField;
}


inline Frustum * GLPanel::GetFrustum()
{
	return &m_world.frustum;
}


inline void GLPanel::SetHeave(GLdouble heave)
{
	m_Heave = heave;
}


inline void GLPanel::SetLateral(GLdouble lateral)
{
	m_Lateral = lateral;
}


inline void GLPanel::SetSurge(GLdouble surge)
{
	m_Surge = surge;
}

