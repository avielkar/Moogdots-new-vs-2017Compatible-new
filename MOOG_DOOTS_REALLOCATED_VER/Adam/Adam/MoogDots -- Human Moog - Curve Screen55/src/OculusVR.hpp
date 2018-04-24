#ifndef OCULUSVR_INCLUDED
#define OCULUSVR_INCLUDED

#include "InputHandlers.hpp"
#include "OpenGL.hpp"
#include "OculusVRDebug.hpp"
#include "OVRCameraFrustum.hpp"
#include "OVRTrackerChaperone.hpp"
#include "Extras/OVR_Math.h"
#include "OVR_CAPI.h"

/*
 * Oculus Rift setup class (as of SDK 1.3.0)
 */
class OculusVR
{
public:
    OculusVR() : m_hmdSession(nullptr),
                 m_debugData(nullptr),
                 m_cameraFrustum(nullptr),
                 m_trackerChaperone(nullptr),
                 m_msaaEnabled(false),
                 m_frameIndex(0),
                 m_sensorSampleTime(0)
    {
    }

    ~OculusVR();
    bool  InitVR();
    bool  InitVRBuffers(int windowWidth, int windowHeight);
    bool  InitNonDistortMirror(int windowWidth, int windowHeight); // create non-distorted mirror if necessary (debug purposes)
    void  DestroyVR();
    const ovrSizei GetResolution() const;
    void  OnRenderStart();
	const OVR::Matrix4f OnEyeRender(int eyeIndex, ovrVector3f eyePos, ovrVector3f centerPos, ovrVector3f upPos, float nearZ, float farZ);
	
	//Get the eye orientation by pitch raw and yaw.
	const ovrQuatf GetEyeOrientationQuaternion(int eyeIndex)
	{
		return m_eyeRenderPose[eyeIndex].Orientation;
	}
	OVR::Matrix4f LookAtRH(const OVR::Vector3f& eye, const OVR::Vector3f& at, const OVR::Vector3f& up);
    void  OnEyeRenderFinish(int eyeIndex);
    const OVR::Matrix4f GetEyeMVPMatrix(int eyeIdx) const;
    void  SubmitFrame();

    void  BlitMirror(ovrEyeType numEyes=ovrEye_Count, int offset = 0);   // regular OculusVR mirror view
    void  OnNonDistortMirrorStart();        // non-distorted mirror rendering start (debug purposes)
    void  BlitNonDistortMirror(int offset); // non-distorted mirror rendering (debug purposes)

    void  OnKeyPress(KeyCode key);
	void  OnCtrlKeyPress(KeyCode key);
    void  CreateDebug();
    void  UpdateDebug();
    void  RenderDebug();
    void  RenderTrackerFrustum();
    void  RenderTrackerChaperone();
    bool  IsDebugHMD() const { return (m_hmdDesc.AvailableHmdCaps & ovrHmdCap_DebugDevice) != 0; }
    void  ShowPerfStats(ovrPerfHudMode statsMode);
    void  SetMSAA(bool val) { m_msaaEnabled = val; }
    bool  MSAAEnabled() const { return m_msaaEnabled; }
	OVR::Matrix4f GetProjectionMatrix(int eyeIndex, float zDistanceFromScreen, float* fixationPoint)
	{
		ovrVector3f vec;
		ovrVector3f vec2;
		ovrVector3f vec3;
		vec.x = 0;
		vec.y = 1;
		vec.z = 0;

		vec2.x = 0;
		vec2.y = 0;
		vec2.z = zDistanceFromScreen;

		vec3.x = fixationPoint[0];
		vec3.y = fixationPoint[1];
		vec3.z = fixationPoint[2];

		return m_projectionMatrix[eyeIndex] * m_eyeOrientation[eyeIndex] * LookAtRH(vec2, vec3, vec);
	}

	OVR::Matrix4f OnTranslate(double offsetX, double offsetY, double offsetZ)
	{
		OVR::Vector3f translateVec;
		translateVec.x = offsetX;
		translateVec.y = offsetY;
		translateVec.z = offsetZ;
		return OVR::Matrix4f::Translation(-translateVec);
	}
private:
    // A buffer struct used to store eye textures and framebuffers.
    // We create one instance for the left eye, one for the right eye.
    // Final rendering is done via blitting two separate frame buffers into one render target.
    struct OVRBuffer
    {  
        OVRBuffer(const ovrSession &session, int eyeIdx);
        void OnRender();
        void OnRenderFinish();
        void SetupMSAA(); 
        void OnRenderMSAA();
        void OnRenderMSAAFinish();
        void Destroy(const ovrSession &session);

        ovrSizei   m_eyeTextureSize;
        GLuint     m_eyeFbo      = 0;
        GLuint     m_eyeTexId    = 0;
        GLuint     m_depthBuffer = 0;

        GLuint m_msaaEyeFbo   = 0;   // framebuffer for MSAA texture
        GLuint m_eyeTexMSAA   = 0;   // color texture for MSAA
        GLuint m_depthTexMSAA = 0;   // depth texture for MSAA

        ovrTextureSwapChain m_swapTextureChain = nullptr;
    };

    // data and buffers used to render to HMD
    ovrSession        m_hmdSession;
    ovrHmdDesc        m_hmdDesc;
    ovrEyeRenderDesc  m_eyeRenderDesc[ovrEye_Count];
    ovrPosef          m_eyeRenderPose[ovrEye_Count];
    ovrVector3f       m_hmdToEyeOffset[ovrEye_Count];
    OVRBuffer        *m_eyeBuffers[ovrEye_Count];

    OVR::Matrix4f     m_projectionMatrix[ovrEye_Count];
    OVR::Matrix4f     m_eyeOrientation[ovrEye_Count];
    OVR::Matrix4f     m_eyePose[ovrEye_Count];

    // frame timing data and tracking info
    double            m_frameTiming;
    ovrTrackingState  m_trackingState;

    // mirror texture used to render HMD view to OpenGL window
    ovrMirrorTexture     m_mirrorTexture;
    ovrMirrorTextureDesc m_mirrorDesc;

    // debug non-distorted mirror texture data
    GLuint            m_nonDistortTexture;
    GLuint            m_nonDistortDepthBuffer;
    GLuint            m_mirrorFBO;
    GLuint            m_nonDistortFBO;
    int               m_nonDistortViewPortWidth;
    int               m_nonDistortViewPortHeight;
    bool              m_msaaEnabled;
    long long         m_frameIndex;
    double            m_sensorSampleTime;

    // debug hardware output data
    OculusVRDebug    *m_debugData;

    // debug camera frustum renderer
    OVRCameraFrustum *m_cameraFrustum;

    // debug tracker chaperone
    OVRTrackerChaperone *m_trackerChaperone;
};


#endif