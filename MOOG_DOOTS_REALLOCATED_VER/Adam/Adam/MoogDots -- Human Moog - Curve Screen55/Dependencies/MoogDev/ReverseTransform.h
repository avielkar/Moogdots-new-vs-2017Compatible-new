// ReverseTransform.h: interface for the ReverseTransform class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REVERSETRANSFORM_H__B4713392_D0AE_4B22_995A_EDDBB445EE3F__INCLUDED_)
#define AFX_REVERSETRANSFORM_H__B4713392_D0AE_4B22_995A_EDDBB445EE3F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class ReverseTransform  
{
public:
	ReverseTransform();
	virtual ~ReverseTransform();

	bool ReverseTransformEnglish(double* dActuator_Actual_Leg_Lengths, double* dDOF);
	bool ReverseTransformMetric(double* dActuator_Actual_Leg_Lengths, double* dDOF);
};

#endif // !defined(AFX_REVERSETRANSFORM_H__B4713392_D0AE_4B22_995A_EDDBB445EE3F__INCLUDED_)
