#ifndef AirPhysicsSystem_h__
#define AirPhysicsSystem_h__

#include "AirEngineHeader.h"

namespace	Air{
	namespace	Physics{
		class	SVO;
		class	ENGINE_EXPORT	System :	
			public	Common::IFactoryManager,
			public	Singleton<System>
		{
		public:
			System();

			virtual	U1		Initialization();
			virtual	U1		Release();
			
			void			Update(const FrameTime& frameTime);
			void			UpdateSVO(U32* svoData,U32 uiDepth,float fScale);
			U1				CollisionDetect(const Float3& p,Float3& v);
			U1				Simulation(Float3& p,Float3& v,U1 bGravity	=	false);
			U1				Silumation(Float3& p,float fRadius,float fHeight,Float3& v,U1 bGravity	=	false);
		public:
			inline const Float3&	GetGravity(){
				return m_vGravity;
			};
			inline	void			SetGravity(const Float3& v){
				m_vGravity	=	v;
			};
			inline	void			SetGravity(float x,float y,float z){
				m_vGravity.x	=	x;
				m_vGravity.y	=	y;
				m_vGravity.z	=	z;
			};
		public:
			SVO*			m_pSVO;
			Float3			m_vGravity;
			
		};
	}
	typedef	Physics::System	PhysicsSystem;
}
#endif // AirPhysicsSystem_h__