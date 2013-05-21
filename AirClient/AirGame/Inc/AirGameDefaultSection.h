#ifndef AirGameDefaultSection_h__
#define AirGameDefaultSection_h__

#include "AirGameHeader.h"

namespace	Air{
	namespace	Game{
		class	GAME_EXPORT	DefaultSection	:public	Section{
		public:
			static AString	ProductTypeName;
			DefaultSection(CAString& strName);

			virtual	U1	Create();
			virtual	U1	Destroy();
			virtual	void	OnActorState(enumActorState	state,enumMoveState mstate);
			virtual	void	OnAction(enumAction	action);
			virtual	void	OnGameState(enumGameState	state);

			void	__stdcall	OnESC(const Key& k);
			void	__stdcall	OnFire(const Key& k);
			AnimationModel*	m_pModel;
		};
		class	GAME_EXPORT	EditorSection	:public	Section{
		public:
			static AString	ProductTypeName;
			EditorSection(CAString& strName);

			virtual	U1	Create();
			virtual	U1	Destroy();

			virtual	Control*	OnCreateControl();		
		};
	}
}
#endif // AirGameDefaultSection_h__
